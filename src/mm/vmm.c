/*
 * =====================================================================================
 *
 *       Filename:  vmm.c
 *
 *    Description:  虚拟内存管理
 *
 *        Version:  1.0
 *        Created:  2013年09月26日 19时59分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "string.h"
#include "printk.h"
#include "panic.h"
#include "vmm.h"
#include "pmm.h"
#include "idt.h"

// 页目录地址
uint32_t *page_directory = (uint32_t *)PAGE_DIR_VIRTUAL_ADDR;

// 页表地址
uint32_t *page_tables = (uint32_t *)PAGE_TABLE_VIRTUAL_ADDR;

// 当前页目录地址
page_directory_t *current_directory;

// 页错误中断的函数处理
void page_fault(registers_t *regs);

void init_vmm()
{
	int i;
	uint32_t cr0;

	// 注册页错误中断的处理函数(14 是页故障的中断号)
	register_interrupt_handler(14, &page_fault);

	// 创建一个页目录，此时未开启分页
	page_directory_t *pd = (page_directory_t *)pmm_alloc_page();

	// 清空整个页目录的数据为 0
	bzero(pd, 0x1000);

	// 页目录第一项我们来映射最开始的 4 MB 内存
	pd[0] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;

	uint32_t *pt = (uint32_t *)(pd[0] & PAGE_MASK);

	// 乘以 0x1000 等价于左移 12 位
	// 我们通过这个循环很巧妙的将物理内存的前 4MB 映射的和启用分页后的虚拟地址相同
	// 即就是我们开启分页后，至少对前 4MB 内存的访问使用原先的物理地址即可
	// 从某种意义上来说，按照我们的设计，内核自己访问的虚拟地址和物理地址相同
	for (i = 0; i < 1024; i++) {
	      pt[i] = i * 0x1000 | PAGE_PRESENT | PAGE_WRITE;
	}

	// 我们再映射 4G 地址空间最后的地址
	pd[1022] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
	pt = (uint32_t*) (pd[1022] & PAGE_MASK);
	
	bzero(pt, 0x1000);
	
	// 这样做的结果就是 0xFFBFF000 这个页目录的虚拟地址正好被映射到了页目录实际的物理地址
	// 0xFFBFF 拆开就是 1111111110 1111111111 其正好是 1022 1023
	// 即就是第 1022 号 页表的第 1023 项，正好是物理页目录的地址
	pt[1023] = (uint32_t)pd | PAGE_PRESENT | PAGE_WRITE;

	// 页表虚拟地址 0xFFC00000 就是 1111111111 0000000000 转换后就是 1023 0
	// 即就是第 1023 号页表的第 0 项，正好是内核 4 MB 页表的地址
	// 好奇葩的设计，我快被绕晕了
	pd[1023] = (uint32_t)pd | PAGE_PRESENT | PAGE_WRITE;
	
	// 以上的设置解决了分页模型下我们可以通过 0xFFBFF000访问到虚拟页目录
	// 我们也能用 0xFFC00000 访问到 0 号页表的地址
	// 而且，我们使用 0xFFC00000 这个虚拟地址 页表 部分的偏移
	// 就可以顺利找到页目录每一项所指向的页表数据块了

	// 设置好当前的页目录地址
	switch_page_directory(pd);

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	// PMM_STACK_ADDR 0xFF000000 物理内存管理的栈地址
	// 必须在分页模式开启之前给该项分配内存，否则会直接引起异常
	// 异常的原因是调用了 pmm_free_page 后触发 map 调用，map 会调用 pmm_alloc_page
	// 但是 pmm_alloc_age 此时无法找到可用内存，导致触发下溢条件 panic 结束
	// 这一步找到了该地址应该在页目录的项目
	uint32_t pt_idx = PAGE_DIR_IDX((PMM_STACK_ADDR >> 12));

	// 给该项分配内存
	page_directory[pt_idx] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;

	// 给该内存页数据清 0，注意这里乘以 1024 相当于左移 10 位
	// 注意这里数组的类型是 4 字节类型！所以计算数组地址偏移的时候会乘以 4
	// 所以我们只要乘以 1024 ，一共乘以 4096，等于左移了 12 位
	// 这样就自动计算出了目标地址的 页表 自身的数据结构的偏移地址
	bzero((void *)page_tables[pt_idx * 1024], 0x1000);

	// 设置分页模式开启标记
	mm_paging_active = 1;
}

void switch_page_directory(page_directory_t *pd)
{
	current_directory = pd;
	asm volatile ("mov %0, %%cr3" : : "r" (pd));
}

void map(uint32_t va, uint32_t pa, uint32_t flags)
{
	uint32_t virtual_page = va / 0x1000;
	uint32_t pt_idx = PAGE_DIR_IDX(virtual_page);

	// 找到虚拟地址 va 对应的描述表，如果它没被使用的话
	if (page_directory[pt_idx] == 0) {
		page_directory[pt_idx] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
		// 这里不再解释，原理同上
		bzero((void *)page_tables[pt_idx * 1024], 0x1000);
	}

	// 创建好以后设置页表项，让这个地址所处的那一页内存指向目标物理内存页
	page_tables[virtual_page] = (pa & PAGE_MASK) | flags;
}

void unmap(uint32_t va)
{
	uint32_t virtual_page = va / 0x1000;

	page_tables[virtual_page] = 0;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

char get_mapping(uint32_t va, uint32_t *pa)
{
	uint32_t virtual_page = va / 0x1000;
	uint32_t pt_idx = PAGE_DIR_IDX(virtual_page);

	// 如果当前地址没有被映射直接返回 0
	if (page_directory[pt_idx] == 0) {
	      return 0;
	}
	
	// 如果地址有效而且指针不为NULL，则返回地址
	if (page_tables[virtual_page] != 0 && pa) {
		 *pa = page_tables[virtual_page] & PAGE_MASK;
		return 1;
	}

	return -1;
}

void page_fault(registers_t *regs)
{
	uint32_t cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printk("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
	printk("Error code: %x\n", regs->err_code);

	// bit 0 为 0 指页面不存在内存里
	if ( !(regs->err_code & 0x1)) {
		printk_color(rc_black, rc_red, "Because the page wasn't present.\n");
	}
	// bit 1 为 0 表示读错误，为 1 为写错误
	if (regs->err_code & 0x2) {
		printk_color(rc_black, rc_red, "Write error.\n");
	} else {
		printk_color(rc_black, rc_red, "Read error.\n");
	}
	// bit 2 为 1 表示在用户模式打断的，为 0 是在内核模式打断的
	if (regs->err_code & 0x4) {
		printk_color(rc_black, rc_red, "In user mode.\n");
	} else {
		printk_color(rc_black, rc_red, "In kernel mode.\n");
	}
	// bit 3 为 1 表示错误是由保留位覆盖造成的
	if (regs->err_code & 0x8) {
		printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
	}
	// bit 4 为 1 表示错误发生在取指令的时候
	if (regs->err_code & 0x10) {
		printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
	}

	panic("Page Error:");
	
	while (1);
}


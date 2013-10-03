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

	// 注册页错误中断的处理函数( 14 是页故障的中断号)
	register_interrupt_handler(14, &page_fault);

	// 创建一个页目录，此时未开启分页
	page_directory_t *pd = (page_directory_t*)pmm_alloc_page();

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

	// 我们再映射 4G 地址空间最后的 8 MB 内存，这是较前 4 MB
	pd[1022] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
	pt = (uint32_t*) (pd[1022] & PAGE_MASK);
	
	bzero(pt, 0x1000);

	// 这是最后的 4 MB了
	pt[1023] = (uint32_t)pd | PAGE_PRESENT | PAGE_WRITE;

	// 页表最后的地址循环回了页目录自己....
	// 好奇葩的设计，我快被绕晕了
	pd[1023] = (uint32_t)pd | PAGE_PRESENT | PAGE_WRITE;

	// 设置好当前的页目录地址
	switch_page_directory(pd);

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	// 我们需要设置物理内存管理的页表，都则 pmm_free_page 函数就错误了
	uint32_t pt_idx = PAGE_DIR_IDX((PMM_STACK_ADDR >> 12));
	page_directory[pt_idx] = pmm_alloc_page() | PAGE_PRESENT | PAGE_WRITE;
	bzero((void *)page_tables[pt_idx * 1024], 0x1000);

	// 设置分页模式开启标记
	pmm_paging_active = 1;
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
		bzero((void *)page_tables[pt_idx * 1024], 0x1000);
	}

	// 创建好以后设置页表项
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

	if (page_directory[pt_idx] == 0) {
	      return 0;
	}

	if (page_tables[virtual_page] != 0) {
		if (pa) *pa = page_tables[virtual_page] & PAGE_MASK;
		return 1;
	}

	return -1;
}

void page_fault(registers_t *regs)
{
	uint32_t cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printk("Page fault at 0x%x, faulting address 0x%x\n", regs->eip, cr2);
	printk("Error code: %x\n", regs->err_code);
	panic("");
	
	while (1);
}


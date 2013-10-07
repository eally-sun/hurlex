/*
 * =====================================================================================
 *
 *       Filename:  pmm.c
 *
 *    Description:  页内存管理
 *
 *        Version:  1.0
 *        Created:  2013年09月26日 19时45分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "common.h"
#include "printk.h"
#include "panic.h"
#include "vmm.h"
#include "pmm.h"

#define SHOW_MEM_MAP

// 当前申请到的位置
uint32_t pmm_stack_loc = PMM_STACK_ADDR;

// 内存申请的界限位置
uint32_t pmm_stack_max = PMM_STACK_ADDR;

// 没有开启分页机制时的内存管理方案采用的管理指针
uint32_t pmm_location;

// 是否开启内存分页
char mm_paging_active = 0;

void init_pmm(uint32_t start)
{
	// 因为起始地址必须按照 4KB 对齐，所以和掩码做与运算
	// 考虑到减少地址值会覆盖原先的数据，所以先增加 4KB 再与运算
	// 此处至多有 4KB 的内存浪费，但是效率更重要
	pmm_location = (start + 0x1000) & PAGE_MASK;
}

void init_page_pmm(multiboot_t *mboot_ptr)
{
	#ifdef SHOW_MEM_MAP
	
	// 打印 BIOS 反馈的内存布局
	printk("1 MB under:\n");
	printk("mem_lower: 0x%X\n", mboot_ptr->mem_lower);
	printk("mem_upper: 0x%X\n\n", mboot_ptr->mem_upper);

	mmap_entry_t *mmap;
	printk("mmap_addr = 0x%X, mmap_length = 0x%X\n\n", (unsigned)mboot_ptr->mmap_addr, (unsigned)mboot_ptr->mmap_length);
	for (mmap = (mmap_entry_t *) mboot_ptr->mmap_addr;
		(unsigned long) mmap < mboot_ptr->mmap_addr + mboot_ptr->mmap_length; 
		mmap = (mmap_entry_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size))) {
	      printk("  size = 0x%X, base_addr = 0x%X%X,"
				      " length = 0x%X%X, type = 0x%X\n",
				      (unsigned)mmap->size,
				      (unsigned)mmap->base_addr_high,
				      (unsigned)mmap->base_addr_low,
				      (unsigned)mmap->length_high,
				      (unsigned)mmap->length_low,
				      (unsigned)mmap->type);
	}
	printk("\n");
	#endif
	
	uint32_t i = mboot_ptr->mmap_addr;
	while (i < mboot_ptr->mmap_addr + mboot_ptr->mmap_length) {
		mmap_entry_t *map_entry = (mmap_entry_t *)i;
		// 如果是可用内存(按照协议，1 表示可用内存，其它数字指保留区域)
		if (map_entry->type == 1) {
			// 把这些内存段，按页存储到页管理栈里
			// TODO 警告，我们操作系统装载所在内存也被加入!
			uint32_t j = map_entry->base_addr_low;
			while (j < map_entry->base_addr_low + map_entry->length_low) {
				pmm_free_page(j);
				j += 0x1000;
			}
		}
		// multiboot 规范中大小这项数据不包含指针自身的大小
		// 所以我们要再加上一个指针大小，真是奇怪的规范 - -
		i += map_entry->size + sizeof(uint32_t);
	}
}

uint32_t pmm_alloc_page()
{
	// 根据是否开启分页决定对内存的管理策略
	if (mm_paging_active) {
		// 确认栈地址没有下溢
		// 在分页开启以后，需要先收集空闲内存放入管理栈才可以进一步申请内存
		// 否则之掉调用申请内存页会出错
		if (pmm_stack_loc == PMM_STACK_ADDR) {
			panic("Error: Out of Memory!");
		}
		// 相当于出栈
		pmm_stack_loc -= sizeof(uint32_t);
		uint32_t *stack = (uint32_t *)pmm_stack_loc;

		return *stack;
	}

	// 0x1000 即 4096D，每页内存 4 KB
	pmm_location += 0x1000;
	return pmm_location;
}

void pmm_free_page(uint32_t p)
{
	if (p < pmm_location) {
		return;
	}
	
	// 此处意为存储空闲页面的内存页写满了
	// 需要映射一页内存来存储索引
	if (pmm_stack_max <= pmm_stack_loc) {
		map(pmm_stack_max, p, PAGE_PRESENT | PAGE_WRITE);
		pmm_stack_max += 4096;
	} else {
		// 将这个空闲的内存页地址存入管理栈
		uint32_t *stack = (uint32_t*)pmm_stack_loc;
		*stack = p;
		pmm_stack_loc += sizeof(uint32_t);
	}
}


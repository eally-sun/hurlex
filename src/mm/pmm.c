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
#include "panic.h"
#include "vmm.h"
#include "pmm.h"

uint32_t pmm_stack_loc = PMM_STACK_ADDR;
uint32_t pmm_stack_max = PMM_STACK_ADDR;
uint32_t pmm_location;
char 	 pmm_paging_active = 0;

void init_pmm(uint32_t start)
{
	pmm_location = (start + 0x1000) & PAGE_MASK;
}

uint32_t pmm_alloc_page()
{
	if (pmm_paging_active) {
		if (pmm_stack_loc == PMM_STACK_ADDR) {
			panic("Error: Out of Memory!");
		}
		pmm_stack_loc -= sizeof (uint32_t);
		uint32_t *stack = (uint32_t *)pmm_stack_loc;

		return *stack;
	}

	return pmm_location += 0x1000;
}

void pmm_free_page (uint32_t p)
{
	if (p < pmm_location) {
		return;
	}

	if (pmm_stack_max <= pmm_stack_loc) {
		map(pmm_stack_max, p, PAGE_PRESENT | PAGE_WRITE);
		pmm_stack_max += 4096;
	} else {
		uint32_t *stack = (uint32_t*)pmm_stack_loc;
		*stack = p;
		pmm_stack_loc += sizeof (uint32_t);
	}
}


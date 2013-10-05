/*
 * =====================================================================================
 *
 *       Filename: hx_ main.c
 *
 *    Description:  hurlex 内核的入口函数
 *
 *        Version:  1.0
 *        Created:  2013年07月24日 19时31分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "multiboot.h"
#include "printk.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "panic.h"
#include "elf.h"
#include "pmm.h"
#include "vmm.h"

elf_t kernel_elf;

int hx_main(multiboot_t *mboot_ptr)
{
	init_gdt();
	init_idt();

	monitor_clear();
	printk_color(rc_black, rc_green, "Hello, hurlex kernel!\n");
	printk_color(rc_black, rc_red, "This is a simple OS kernel, just for study.\nYou can copy it freely!\n\n");
	
	//init_timer(20);
	//asm volatile("sti");
		
	//kernel_elf = elf_from_multiboot(mboot_ptr);
	//panic("Testing panic mechanism");
	
	init_pmm(mboot_ptr->mem_upper);
	init_vmm();
	init_page_pmm(mboot_ptr);

	printk_color(rc_black, rc_blue, "Start Paging Mode...\n");

	while (1);	

	return 0;
}


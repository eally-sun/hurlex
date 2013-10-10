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
#include "heap.h"

elf_t kernel_elf;

int hx_main(multiboot_t *mboot_ptr)
{
	kernel_elf = elf_from_multiboot(mboot_ptr);
	
	init_gdt();
	init_idt();

	monitor_clear();
	printk_color(rc_black, rc_red, "**************************\n");
	printk_color(rc_black, rc_red, "*  Hello, hurlex kernel! *\n");
	printk_color(rc_black, rc_red, "**************************\n\n");
	printk_color(rc_black, rc_green, "This is a simple OS kernel, just for study.\nYou can copy it freely!\n\n");
	
	//init_timer(20);
	//asm volatile("sti");

	init_pmm(mboot_ptr);
	init_vmm();
	init_page_pmm(mboot_ptr);

	init_heap();

	printk_color(rc_black, rc_cyan, "Start Paging Mode ...\n\n");
	
	printk_color(rc_black, rc_magenta, "Kernel heap created ...\n\n");
	printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

	void *addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	void *addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	void *addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	void *addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);
	printk("free mem in 0x%X\n\n", addr4);
	kfree(addr4);

	return 0;
}


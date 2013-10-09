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
	init_gdt();
	init_idt();

	monitor_clear();
	printk_color(rc_black, rc_green, "Hello, hurlex kernel!\n\n");
	printk_color(rc_black, rc_red, "This is a simple OS kernel, just for study.\nYou can copy it freely!\n\n");
	
	//init_timer(20);
	//asm volatile("sti");
		
	//kernel_elf = elf_from_multiboot(mboot_ptr);
	//panic("Testing panic mechanism");
	
	init_pmm(mboot_ptr->mem_upper);
	init_vmm();
	init_page_pmm(mboot_ptr);

	init_heap();

	printk_color(rc_black, rc_cyan, "Start Paging Mode & Init Heap ...\n");

	void * addr1 = kmalloc(50);
	printk("kmalloc 50 mem in 0x%X\n", kmalloc(50));
	void * addr2 = kmalloc(500);
	printk("kmalloc 500 mem in 0x%X\n", kmalloc(500));
	void * addr3 = kmalloc(5000);
	printk("kmalloc 5000 mem in 0x%X\n", kmalloc(5000));

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);

	printk_color(rc_black, rc_red, "kernel heap succeed!\n");

	while (1);	

	return 0;
}


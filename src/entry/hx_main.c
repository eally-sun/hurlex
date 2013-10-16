/*
 * =====================================================================================
 *
 *       Filename:  hx_ main.c
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
#include "thread.h"
#include "scheduler.h"

// 定义 elf 相关信息数据
elf_t kernel_elf;

// 内核线程函数
int thread_func(void *arg)
{
	printk("New Thread is Running!\n");

	return (int)arg;
}

int hx_main(multiboot_t *mboot_ptr)
{
	// 从 GRUB 提供的信息中获取到内核符号表和代码地址信息
	kernel_elf = elf_from_multiboot(mboot_ptr);
	
	// 初始化全局段描述符表
	init_gdt();

	// 初始化全局中断描述符表
	init_idt();

	// 清屏
	monitor_clear();
	printk_color(rc_black, rc_red, "**************************\n");
	printk_color(rc_black, rc_red, "*  Hello, hurlex kernel! *\n");
	printk_color(rc_black, rc_red, "**************************\n\n");
	printk_color(rc_black, rc_green, "This is a simple OS kernel, just for study.\nYou can copy it freely!\n\n");
	
	// 初始化物理内存管理
	init_pmm(mboot_ptr);

	// 初始化虚拟内存管理
	init_vmm();

	// 初始化物理内存可用页
	init_page_pmm(mboot_ptr);

	// 初始化内核态堆管理
	init_heap();

	printk_color(rc_black, rc_cyan, "Start Paging Mode ...\n\n");
	
	printk_color(rc_black, rc_magenta, "Kernel heap created ...\n\n");
/*	printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

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
*/

	// 初始化内核线程调度
	init_scheduler(init_threading());
	
	// 申请 1 KB 内存作为内核线程的栈
	void *thread_stack = kmalloc(0x400);

	// 创建内核线程，注意栈地址从高往低增长
	kernel_thread(thread_func, 0, thread_stack + 0x400);

	// 初始化时钟中断
	init_timer(20);

	// 解除对 INTR 中断的屏蔽
	asm volatile("sti");

	printk("Old Thread is it!\n");

	while (1);

	return 0;
}


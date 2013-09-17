/*
 * =====================================================================================
 *
 *       Filename:  main.c
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
#include "monitor.h"
#include "gdt.h"

int hx_main(multiboot_t *mboot_ptr)
{
	init_gdt();
	monitor_clear();
	monitor_write_color("Hello, hurlex kernel!\n", rc_black, rc_green);
	monitor_write_color("This is a simple OS kernel, just for study.\n", rc_black, rc_red);
	monitor_write_color("You can copy it freely!\n", rc_black, rc_red);
	
	return 0;
}

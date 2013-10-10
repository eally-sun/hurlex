/*
 * =====================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  PIT 相关
 *
 *        Version:  1.0
 *        Created:  2013年07月27日 09时27分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "timer.h"
#include "printk.h"
#include "idt.h"
#include "scheduler.h"

void timer_callback(registers_t *regs)
{
	//static uint32_t tick = 0;
	//tick++;
	//printk_color(rc_black, rc_red, "Tick: %d\n", tick);
	
	// 进行内核线程的调度
	schedule();
}

void init_timer(uint32_t frequency)
{
	// 注册时间相关的处理函数
	register_interrupt_handler(IRQ0, timer_callback);

	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	uint32_t divisor = 1193180 / frequency;

	outb(0x43, 0x36);

	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);

	outb(0x40, low);
	outb(0x40, hign);
}

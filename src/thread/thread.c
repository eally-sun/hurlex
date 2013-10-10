/*
 * =====================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  内核线程的实现
 *
 *        Version:  1.0
 *        Created:  2013年10月10日 15时53分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "thread.h"
#include "string.h"
#include "heap.h"
#include "printk.h"
#include "scheduler.h"

// 当前执行线程
thread_t *current_thread;

// 下一个线程的 id
uint32_t next_tid = 0;

// 线程退出
static void thread_exit();

extern void _create_thread(int (*)(void *), void *, uint32_t *, thread_t *);

thread_t *init_threading()
{
	thread_t *thread = kmalloc(sizeof(thread_t));
	thread->thread_id = next_tid++;

	current_thread = thread;

	return thread;
}

thread_t *create_thread(int (*fn)(void*), void *arg, uint32_t *stack)
{
	thread_t *thread = kmalloc(sizeof(thread_t));
	bzero(thread, sizeof(thread_t));
	thread->thread_id = next_tid++;

	*(--stack) = (uint32_t)arg;
	*(--stack) = (uint32_t)&thread_exit;
	*(--stack) = (uint32_t)fn;

	thread->esp = (uint32_t)stack;
	thread->ebp = 0;
	thread->eflags = 0x200;
	thread_is_ready(thread);

	return thread;
}

void thread_exit()
{
	register uint32_t val asm ("eax");

	printk("Thread exited with value %d\n", val);

	while (1);
}


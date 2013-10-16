/*
 * =====================================================================================
 *
 *       Filename:  scheduler.h
 *
 *    Description:  内核的线程调度
 *
 *        Version:  1.0
 *        Created:  2013年10月10日 16时07分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_SCHEDULER_H_
#define INCLUDE_SCHEDULER_H_

#include "common.h"
#include "thread.h"

typedef
struct thread_list {
	thread_t *thread;
	struct thread_list *next;
} thread_list_t;

// 初始化线程调度
void init_scheduler(thread_t *initial_thread);

// 线程就绪
void thread_is_ready(thread_t *t);

// 线程非就绪
void thread_not_ready(thread_t *t);

// 调度线程
void schedule();

#endif 	// INCLUDE_SCHEDULER_H_

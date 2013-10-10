/*
 * =====================================================================================
 *
 *       Filename:  scheduler.c
 *
 *    Description:  线程调度函数
 *
 *        Version:  1.0
 *        Created:  2013年10月10日 16时11分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "scheduler.h"
#include "heap.h"

// 就绪队列
thread_list_t *ready_queue = 0;

// 当前运行的线程
thread_list_t *current_thread = 0;

void init_scheduler(thread_t *initial_thread)
{
	current_thread = (thread_list_t *)kmalloc(sizeof(thread_list_t *));
	current_thread->thread = initial_thread;
	current_thread->next = 0;
	ready_queue = 0;
}

void thread_is_ready(thread_t *t)
{
	// 为新增加的线程创建数据结构
	thread_list_t *item = (thread_list_t *)kmalloc(sizeof(thread_list_t *));
	item->thread = t;
	item->next = 0;

	// 假若等待队列当前为空则直接赋值
	// 否则便利链表插入新的线程到队尾
	if (!ready_queue) {
		ready_queue = item;
	} else {
		thread_list_t *iterator = ready_queue;
		while (iterator->next) {
		      iterator = iterator->next;
		}
		iterator->next = item;
	}
}

void thread_not_ready(thread_t *t)
{
	thread_list_t *iterator = ready_queue;

	// 在就绪队列里找到并删除该线程，并释放空间
	if (iterator->thread == t) {
		ready_queue = iterator->next;
		kfree(iterator);
		return;
	}

	while (iterator->next) {
		if (iterator->next->thread == t) {
			thread_list_t *tmp = iterator->next;
			iterator->next = tmp->next;
			kfree(tmp);
		}
		iterator = iterator->next;
	}
}

void schedule ()
{
	// 就绪队列为空就直接返回
	if (!ready_queue) {
		return;
	}
	thread_list_t *iterator = ready_queue;

	while (iterator->next) {
	      iterator = iterator->next;
	}

	// 把老的线程插入到队尾
	iterator->next = current_thread;
	current_thread->next = 0;

	thread_list_t *new_thread = ready_queue;

	// 队头移除老的进程
	ready_queue = ready_queue->next;

	// 切换到新的线程执行
	switch_thread(new_thread);
}

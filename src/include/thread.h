/*
 * =====================================================================================
 *
 *       Filename:  thread.h
 *
 *    Description:  内核线程的定义
 *
 *        Version:  1.0
 *        Created:  2013年10月10日 15时52分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_

#include "common.h"

typedef struct thread_t {
  uint32_t esp;
  uint32_t ebp;
  uint32_t ebx;
  uint32_t esi;
  uint32_t edi;
  uint32_t eflags;
  uint32_t thread_id;
} thread_t;

// 内核线程链表
struct thread_list;

// 初始化内核线程创建环境
thread_t *init_threading();

// 创建内核线程
thread_t *create_thread(int (*fn)(void*), void *arg, uint32_t *stack);

// 切换线程
void switch_thread(struct thread_list *next);



#endif 	// INCLUDE_THREAD_H_

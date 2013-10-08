/*
 * =====================================================================================
 *
 *       Filename:  heap.h
 *
 *    Description:  简单的内核堆管理
 *
 *        Version:  1.0
 *        Created:  2013年10月05日 14时53分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include "common.h"

// 堆开始地址
#define HEAP_START 0xD0000000

// 堆结束地址
//#define HEAP_END   0xFFBFF000
// 此处有错吧？这个地址岂不是重叠覆盖过了物理内存页的栈管理地址
#define HEAP_END   0xFF000000

typedef struct header {
	struct header *prev;
	struct header *next;
	uint32_t allocated : 1;
	uint32_t length : 31;
} header_t;

// 初始化堆
void init_heap();

// 内存申请
void *kmalloc(uint32_t len);

// 内存释放
void kfree(void *p);

#endif 	// INCLUDE_HEAP_H_

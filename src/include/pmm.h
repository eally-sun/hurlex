/*
 * =====================================================================================
 *
 *       Filename:  pmm.h
 *
 *    Description:  页面内存管理
 *
 *        Version:  1.0
 *        Created:  2013年09月26日 19时43分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

// 物理内存管理的栈地址，我们定在了寻址空间的较高位置
// 我们要能通过这个地址管理所有的内存
#define PMM_STACK_ADDR 	0xFF000000

// 初始化物理内存管理
void init_pmm(uint32_t start);

// 返回一个内存页的物理地址
uint32_t pmm_alloc_page();

// 释放申请的内存
void pmm_free_page(uint32_t p);

// 是否开启内存分页
extern char mm_paging_active;

#endif 	// INCLUDE_PMM_H


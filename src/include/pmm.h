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

#define PMM_STACK_ADDR 	0xFF000000

void init_pmm(uint32_t start);

uint32_t pmm_alloc_page();

void pmm_free_page(uint32_t p);

#endif 	// INCLUDE_PMM_H


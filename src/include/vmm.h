/*
 * =====================================================================================
 *
 *       Filename:  vmm.h
 *
 *    Description:  虚拟内存管理
 *
 *        Version:  1.0
 *        Created:  2013年09月26日 19时57分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_VMM_H
#define INCLUDE_VMM_H

#include "common.h"

#define PAGE_DIR_VIRTUAL_ADDR   	0xFFBFF000
#define PAGE_TABLE_VIRTUAL_ADDR 	0xFFC00000

#define PAGE_DIR_IDX(x) 		((uint32_t)x / 1024)
#define PAGE_TABLE_IDX(x) 		((uint32_t)x % 1024)

#define PAGE_PRESENT   0x1
#define PAGE_WRITE     0x2
#define PAGE_USER      0x4
#define PAGE_MASK      0xFFFFF000

typedef uint32_t page_directory_t;

void init_vmm();

void switch_page_directory(page_directory_t *pd);

void map(uint32_t va, uint32_t pa, uint32_t flags);

void unmap(uint32_t va);

char get_mapping(uint32_t va, uint32_t *pa);

#endif 	// INCLUDE_VMM_H

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

// 页目录虚拟地址
#define PAGE_DIR_VIRTUAL_ADDR   	0xFFBFF000

// 页表虚拟地址
#define PAGE_TABLE_VIRTUAL_ADDR 	0xFFC00000

#define PAGE_DIR_IDX(x) 		((uint32_t)x / 1024)
#define PAGE_TABLE_IDX(x) 		((uint32_t)x % 1024)

/**
 * 以下页表项的一些宏定义
 *
 * 详细的说明按理应该写在文档中，但考虑到理解上的便利，姑且详细注释于此。
 */

/**
 * P--位0是存在（Present）标志，用于指明表项对地址转换是否有效。P=1表示有效；P=0表示无效。
 * 在页转换过程中，如果说涉及的页目录或页表的表项无效，则会导致一个异常。
 * 如果P=0，那么除表示表项无效外，其余位可供程序自由使用。
 * 例如，操作系统可以使用这些位来保存已存储在磁盘上的页面的序号。
 */
#define PAGE_PRESENT   0x1

/** 
 * R/W--位1是读/写（Read/Write）标志。如果等于1，表示页面可以被读、写或执行。
 * 如果为0，表示页面只读或可执行。
 * 当处理器运行在超级用户特权级（级别0、1或2）时，则R/W位不起作用。
 * 页目录项中的R/W位对其所映射的所有页面起作用。
 */
#define PAGE_WRITE     0x2

/**
 * U/S--位2是用户/超级用户（User/Supervisor）标志。
 * 如果为1，那么运行在任何特权级上的程序都可以访问该页面。
 * 如果为0，那么页面只能被运行在超级用户特权级（0、1或2）上的程序访问。
 * 页目录项中的U/S位对其所映射的所有页面起作用。
 */
#define PAGE_USER      0x4

// 页掩码，用于 4KB 对齐
#define PAGE_MASK      0xFFFFF000

typedef uint32_t page_directory_t;

// 初始化虚拟内存管理
void init_vmm();

// 交换地址空间
void switch_page_directory(page_directory_t *pd);

// 使用 flags 指出的页权限，把物理地址 pa 映射到虚拟地址 va
void map(uint32_t va, uint32_t pa, uint32_t flags);

// 取消虚拟地址 va 的物理映射
void unmap(uint32_t va);

// 如果虚拟地址 va 映射到物理地址则返回 1
// 同时如果 pa 不是空指针则把物理地址写入 pa 参数
char get_mapping(uint32_t va, uint32_t *pa);

#endif 	// INCLUDE_VMM_H

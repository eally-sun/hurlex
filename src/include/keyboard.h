/*
 * =====================================================================================
 *
 *       Filename:  keyboard.h
 *
 *    Description:  键盘驱动
 *
 *        Version:  1.0
 *        Created:  2013年10月16日 17时54分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include "common.h"
#include "idt.h"

/*
 * 我们定义的键盘当前状态信息(相关位被设置即为有效)
 *
 * 0: control
 * 1: alt
 * 2: alt-gr
 * 3: left shift
 * 4: right shift
 * 5: caps_lock
 * 6: scroll_lock
 * 7: num_lock
 */
#define CONTROL 0x1
#define ALT 0x2
#define ALTGR 0x4
#define LSHIFT 0x8
#define RSHIFT 0x10
#define CAPSLOCK 0x20
#define SCROLLLOCK 0x40
#define NUMLOCK 0x80

// 8 位的键盘扫描码的接通码使用前7位
// 其最高位置 1 即是其对应的断开码
// 该宏可以和获取的扫描码用来判断一个键是按下还是抬起
#define RELEASED_MASK 0x80

typedef uint8_t key_status_t;

typedef
struct keymap {
	uint8_t scancodes[128]; 	// 键盘扫描码的映射
	uint8_t capslock_scancodes[128];
	uint8_t shift_scancodes[128];
	uint8_t control_map[8];
	key_status_t controls; 		// 键盘的控制状态信息
} keymap_t;

// 初始化键盘驱动
void init_keyboard_driver();

// 更换当前扫描码集
void switch_layout(keymap_t *layout);

// 键盘中断的中断处理函数
void keyboard_handler(pt_regs *regs);

// 从键盘读取一个字符，如果没有可用字符返回 '\0'
char keyboard_getchar();

#endif 	// INCLUDE_KEYBOARD_H_


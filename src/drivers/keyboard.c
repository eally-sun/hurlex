/*
 * =====================================================================================
 *
 *       Filename:  keyboard.c
 *
 *    Description:  键盘驱动程序
 *
 *        Version:  1.0
 *        Created:  2013年10月16日 17时56分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include "keyboard.h"

//Define your keyboard layouts here:
keymap_t us = {
	//normal keys
	{
		/* first row - indices 0 to 14 */
		0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',

		/* second row - indices 15 to 28 */
		'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', //Enter key

		/* 29 = Control, 30 - 41: third row */
		0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',

		/* fourth row, indices 42 to 54, zeroes are shift-keys*/
		0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,

		'*',

		/* Special keys */
		0, //ALT - 56
		' ', // Space - 57
		0, //Caps lock - 58
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
		0, //Num lock - 69
		0, //Scroll lock - 70
		0, //Home - 71
		0, //Up arrow - 72
		0, //Page up - 73
		'-',
		0, //Left arrow - 75
		0,
		0, //Right arrow -77
		'+',
		0, //End - 79
		0, //Dowm arrow - 80
		0, //Page down - 81
		0, //Insert - 82
		0, //Delete - 83
		0, 0, 0,
		0, // F11 - 87
		0, // F12 - 88
		0, // All others undefined
	},

	// caps
	{
		/* first row - indices 0 to 14 */
		0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',

		/* second row - indices 15 to 28 */
		'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',

		/* 29 = Control, 30 - 41: third row */
		0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',

		/* fourth row, indices 42 to 54, zeroes are shift-keys*/
		0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',

		/* Special keys */
		0,   // ALT - 56
		' ', // Space - 57
		0,   // Caps lock - 58
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
		0, // Num lock - 69
		0, // Scroll lock - 70
		0, // Home - 71
		0, // Up arrow - 72
		0, // Page up - 73
		'-',
		0, // Left arrow - 75
		0,
		0, // Right arrow -77
		'+',
		0, // End - 79
		0, // Dowm arrow - 80
		0, // Page down - 81
		0, // Insert - 82
		0, // Delete - 83
		0, 0, 0,
		0, // F11 - 87
		0, // F12 - 88
		0, // All others undefined
	},
	
	// shift
	{
		/* first row - indices 0 to 14 */
		0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',

		/* second row - indices 15 to 28 */
		'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',

		/* 29 = Control, 30 - 41: third row */
		0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',

		/* fourth row, indices 42 to 54, zeroes are shift-keys*/
		0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',

		/* Special keys */
		0,   // ALT - 56
		' ', // Space - 57
		0,   // Caps lock - 58
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1 to F10 - 59 to 68
		0, // Num lock - 69
		0, // Scroll lock - 70
		0, // Home - 71
		0, // Up arrow - 72
		0, // Page up - 73
		'-',
		0, // Left arrow - 75
		0,
		0, // Right arrow -77
		'+',
		0, // End - 79
		0, // Dowm arrow - 80
		0, // Page down - 81
		0, // Insert - 82
		0, // Delete - 83
		0, 0, 0,
		0, // F11 - 87
		0, // F12 - 88
		0, // All others undefined
	},

	// control_map
	{
		29, // Ctrl
		56, // Alt
		0,  // AltGr
		42, // left Shift
		54, // right Shift
		58, // Caps lock
		70, // Scroll lock
		69  // Num lock
	},

	// 键盘的控制键信息初始化为 0 
	0
};

// 当前使用的扫描码集
keymap_t *current_layout;

// 键盘输入的缓冲区队列
char keyboard_buffer[256];

// 键盘输入缓冲区队列的头尾标记
uint32_t keyboard_buffer_start, keyboard_buffer_end;

void init_keyboard_driver()
{
	// 注册键盘中断处理函数
	register_interrupt_handler(IRQ1, &keyboard_handler);
	switch_layout(&us);

	// 初始化队头队尾(其实全局变量默认初始化就是 0，这里为了强调)
	keyboard_buffer_start = 0;
	keyboard_buffer_end = 0;
}

char keyboard_getchar()
{
	// 队列中有数据则取出，否则返回 '\0'
	if (keyboard_buffer_start != keyboard_buffer_end) {
		char ch = keyboard_buffer[keyboard_buffer_start++];
		// 相当于对 256 取模运算
		keyboard_buffer_start &= 255;
		return ch;
	} else {
		return '\0';
	}
}

void switch_layout(keymap_t *layout)
{
	// 设置当前使用的扫描码集
	current_layout = layout;
}

void keyboard_handler(pt_regs *regs)
{
	// 从键盘端口读入按下的键
	uint8_t scancode = inb(0x60);

	// 首先判断如果是按下还是抬起
	if (scancode & RELEASED_MASK) {
		uint32_t i;
		// 我们只检查前 5 个控制键，因为前五位 Ctrl Alt Shift 松开后不保留状态
		// 所以这些按键松开后必须清除按下标记
		for (i = 0; i < 5; i++) {
			if(current_layout->control_map[i] == (scancode & ~RELEASED_MASK)) {
				current_layout->controls &= ~(1 << i);
				return;
			}
		}
	// 我们只处理当键被按下
	} else {
		uint32_t i;
		// 逐一检查各个控制位
		for (i = 0; i < 8; i++) {
			// 如果当前键是控制键，则给相关控制位置 1
			// 如果已有该标志位，则给相关控制位清 0
			if (current_layout->control_map[i] == scancode) {
				if (current_layout->controls & 1 << i) {
				      current_layout->controls &= ~(1 << i);
				} else {
				      current_layout->controls |= (1 << i);
				}
				return;
			}
		}

		uint8_t *scancodes = current_layout->scancodes;
	
		// 如果此时按下了 shift 键，切换到 shift 扫描码
		if ((current_layout->controls & (LSHIFT | RSHIFT)) && !(current_layout->controls & CONTROL)) {
		      scancodes = current_layout->shift_scancodes;
		}
		
		// 如果此时处于大写锁定状态，切换到大写锁定的扫描码
		if ((current_layout->controls & (CAPSLOCK)) && !(current_layout->controls & CONTROL)) {
		      scancodes = current_layout->capslock_scancodes;
		}

		// 如果队列不满则字符入队，否则丢弃
		if (keyboard_buffer_end != keyboard_buffer_start - 1) {
			keyboard_buffer[keyboard_buffer_end++] = scancodes[scancode];
			keyboard_buffer_end &= 255;
		}
	}
}


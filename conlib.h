/*
 * Conlib v1.3
 * Author: Wener
 * E-mail: wenermail@gmail.com
 * Site: http://blog.wener.me
 *
 * 一个windows下控制台编程的辅助库
 *
 */

#include <stdbool.h>

#ifndef conlib_h
#define conlib_h

/* 鼠标按键 */
#define	ConMOUSE_LEFT_BUTTON	1
#define	ConMOUSE_1ST_BUTTON		1
#define	ConMOUSE_RIGHT_BUTTON	2
#define	ConMOUSE_CENTER_BUTTON	4
#define	ConMOUSE_2ND_BUTTON		4
#define	ConMOUSE_3RD_BUTTON		8
#define	ConMOUSE_4TH_BUTTON		10
/* 键盘按键 */
/*
 * 常见:
 * http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
 */
#define ConKEY_BACKSPACE		0x8
#define ConKEY_TAB				0x9
#define ConKEY_ESCAPE			0x1B
#define ConKEY_SPACE			0x20
#define ConKEY_LEFT				0x25
#define ConKEY_UP				0x26
#define ConKEY_RIGHT			0x27
#define ConKEY_DOWN				0x28
#define ConKEY_RETURN			0x0D
#define ConKEY_CONTROL			0x11
#define ConKEY_ALT				0x12

/* 颜色常量 */
enum ConColor
{
	ConRed = 1,
	ConGreen = 2,
	ConBlue = 4,

	ConWhite = ConRed | ConGreen | ConBlue,
	ConBlack = 0
};
/* 键盘控制键状态 */
typedef struct _ConControlKeyState
{
	bool enhanced;
	bool left_ctrl, right_ctrl;
	bool left_alt, right_alt;
	bool shift; 
	bool numlock,capslock,scrolllock; 
} ConControlKeyState; 
/* 鼠标输入 */
enum ConMouseEvent
{
	ConMOUSE_MOVED
	,ConMOUSE_CLICK
	,ConMOUSE_DBCLICK
	,ConMOUSE_HWHEELED
	,ConMOUSE_WHEELED
	,ConMOUSE_RELEASED
}; 
typedef struct _ConMouse
{
	int x, y;
	int button; 
	enum ConMouseEvent event;
	ConControlKeyState *ctrl_key_state;
} ConMouse;
/* 键盘输入 */ 
typedef struct _ConKey
{
	int key;
	char ascii;
	bool state;
	ConControlKeyState *ctrl_key_state;
} ConKey;

/* 初始化 调用前必须先调用该函数 */
void conlib_init();

/* === 输入 === */
/* 获取键值 无输入时返回NULL */
const ConKey* getkey();
/* 获取鼠标 无输入时返回NULL */
const ConMouse* getmouse();

/* === 标题 === */
/* 设置窗口标题 */
bool settitle(const char *title);
/* 获取窗口标题 将标题写入str中 长度为len */
int gettitle(char *str, int len);

/* === 光标 === */
/* 隐藏光标 */
bool hide_cursor();
/* 显示光标 percent 为 0-100 即光标的大小 */ 
bool show_cursor(unsigned int percent);

/* === 色彩 === */
/* 设置文字颜色 */
bool set_text_color(enum ConColor color);
/* 设置背景颜色 */
bool set_background_color(enum ConColor color);

/* === 声音 === */
/* 蜂鸣声 参数为频率和时长 */
void beep(int frequence, int duration);
/* 开关声音 */
void beep_switch(bool turn);
/* 返回声音的开关状态 */
bool beep_state();

/* === 一些辅助函数 === */
/* 延迟 */ 
void delay( unsigned int delayMS); 
/* 暂停输入 同 getch 因为pause为unistd中挂起进程的函数 避免冲突 */
char pausei();
/* 清空屏幕 */
void clrscr();
/* 光标跳转 */ 
void gotoxy(unsigned x, unsigned y);

#endif

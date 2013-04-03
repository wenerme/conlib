/*
 * Conlib v1.3
 * Author: Wener
 * E-mail: wenermail@gmail.com
 * Site: http://blog.wener.me
 *
 * 一个windows下控制台编程的辅助库
 *
 */
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

#include "conlib.h"

#ifndef conlib_c
#define conlib_c

/* 部分系统没有定义这个 */
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
/* 预定义 */
#define DEFAULT_CONSOLE_MODE (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)

/* 内部函数 */
static void conlib_close();
static void update_ctrlkey(unsigned state); 
// 由于console 模式有时会改变 用于确保打开了鼠标输入模式
static void mouse_mode_active();
/* 全局句柄 */ 
static HANDLE hConsoleIn;
static HANDLE hConsoleOut;

/* 键盘鼠标输入 */
static ConMouse cMouse; 
static ConKey cKey;
static ConControlKeyState cCtrlKeyState; 
/* 因为在设置文字或背景的时候都要这两个值 */
static WORD wTextColor;
static WORD wBackgroundColor;

static bool initialized = false;
/* 备份环境设置 */
static DWORD oldConsoleMode;
static CONSOLE_CURSOR_INFO oldConsoleCursorInfo;
static WORD oldTextColor;

/* 蜂鸣的开关 */
static bool beepState;

/* 初始化 */
void conlib_init()
{
	if(true == initialized)
		return; 
		
	initialized = true;
	atexit(conlib_close);
	/* 获取控制台句柄 */ 
	hConsoleIn = GetStdHandle( STD_INPUT_HANDLE); 
	hConsoleOut = GetStdHandle( STD_OUTPUT_HANDLE);
	/* 备份 */
	GetConsoleCursorInfo(hConsoleIn, &oldConsoleCursorInfo);
	GetConsoleMode(hConsoleIn, &oldConsoleMode);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsoleOut, &csbi);
	oldTextColor = csbi.wAttributes;

	/* 设置控制台模式 */
	SetConsoleMode(hConsoleIn, DEFAULT_CONSOLE_MODE); 
	
	/* 默认颜色 */
	wTextColor = ConWhite;
	wBackgroundColor = ConBlack; 
	// 恢复默认颜色
	set_text_color(wTextColor);

	/* 默认打开声音 */
	beep_switch(true);
}
void conlib_close()
{
	if(false == initialized)
		return;
	/* 恢复控制台状态 */
	SetConsoleCursorInfo(hConsoleIn, &oldConsoleCursorInfo);
	SetConsoleMode(hConsoleIn, oldConsoleMode);
	SetConsoleTextAttribute(hConsoleOut, oldTextColor);
	/* 关闭句柄 */
    CloseHandle(hConsoleIn);
    CloseHandle(hConsoleOut);
}
static void update_ctrlkey(unsigned state)
{
	cCtrlKeyState.enhanced = state & ENHANCED_KEY?true:false;
	
	cCtrlKeyState.left_alt = state & LEFT_ALT_PRESSED?true:false;
	cCtrlKeyState.left_ctrl = state & LEFT_CTRL_PRESSED?true:false;
	cCtrlKeyState.right_alt = state & RIGHT_ALT_PRESSED?true:false;
	cCtrlKeyState.right_ctrl = state & RIGHT_CTRL_PRESSED?true:false;
	cCtrlKeyState.shift = state & SHIFT_PRESSED?true:false;

	cCtrlKeyState.capslock = state & CAPSLOCK_ON?true:false;
	cCtrlKeyState.numlock = state & NUMLOCK_ON?true:false;
	cCtrlKeyState.scrolllock = state & SCROLLLOCK_ON?true:false;

}
/* 清空屏幕 */
void clrscr()
{
	system("cls");
}
/* 获取键值 */ 
const ConKey* getkey()
{
	DWORD num;
	KEY_EVENT_RECORD ke;
	INPUT_RECORD e;
	/* 获取事件总数*/ 
	GetNumberOfConsoleInputEvents(hConsoleIn, &num);
	if(num < 1)
		return NULL;

	PeekConsoleInput(hConsoleIn, & e, 1, &num);
	if(e.EventType != MOUSE_EVENT && e.EventType != KEY_EVENT)
	{
		ReadConsoleInput(hConsoleIn, & e, 1, &num); 
		return NULL;
	}else if(e.EventType == MOUSE_EVENT)
		return NULL;
		
	/* 获取输入 */ 
	ReadConsoleInput(hConsoleIn, & e, 1, &num);

	if(e.EventType != KEY_EVENT)
		return NULL;
		
	/* 更新值 */
	ke = e.Event.KeyEvent;
	
	cKey.key = ke.wVirtualKeyCode;
	cKey.state = ke.bKeyDown;
	cKey.ascii = ke.uChar.AsciiChar;
	
	cKey.ctrl_key_state = &cCtrlKeyState;
	update_ctrlkey(ke.dwControlKeyState);

return &cKey;
}
/* 获取鼠标事件 */
const ConMouse* getmouse()
{
	mouse_mode_active();

	DWORD num;
	INPUT_RECORD e;
	MOUSE_EVENT_RECORD me;
	/* 获取事件总数*/ 
	GetNumberOfConsoleInputEvents(hConsoleIn, &num);
	if(num < 1)
		return NULL;
	PeekConsoleInput(hConsoleIn, & e, 1, &num);
	if(e.EventType != MOUSE_EVENT && e.EventType != KEY_EVENT)
	{
		ReadConsoleInput(hConsoleIn, & e, 1, &num); 
		return NULL;
	}else if(e.EventType == KEY_EVENT)
		return NULL;

	/* 获取输入 */
	
	ReadConsoleInput(hConsoleIn, & e, 1, &num); 
	if(e.EventType != MOUSE_EVENT)
		return NULL;
		
	me = e.Event.MouseEvent;
	cMouse.x = me.dwMousePosition.X;
	cMouse.y = me.dwMousePosition.Y;
	update_ctrlkey(me.dwControlKeyState);
	cMouse.ctrl_key_state = &cCtrlKeyState;
	cMouse.event = 0;

	unsigned state = me.dwButtonState;
	int button = 0;
	if(state & FROM_LEFT_1ST_BUTTON_PRESSED)
		button |= ConMOUSE_1ST_BUTTON;
	if(state & FROM_LEFT_2ND_BUTTON_PRESSED)
		button |= ConMOUSE_2ND_BUTTON;
	if(state & FROM_LEFT_3RD_BUTTON_PRESSED)
		button |= ConMOUSE_3RD_BUTTON;
	if(state & FROM_LEFT_4TH_BUTTON_PRESSED)
		button |= ConMOUSE_4TH_BUTTON;
	if(state & RIGHTMOST_BUTTON_PRESSED)
		button |= ConMOUSE_RIGHT_BUTTON;
	cMouse.button = button;

	switch(me.dwEventFlags)
	{
		case DOUBLE_CLICK:
			cMouse.event = ConMOUSE_CLICK;
		break;
		case MOUSE_HWHEELED:
			cMouse.event = ConMOUSE_HWHEELED;
		break;
		case MOUSE_MOVED:
			cMouse.event = ConMOUSE_MOVED;
		break;
		case MOUSE_WHEELED:
			cMouse.event = ConMOUSE_WHEELED;
		break;

		/* 处理不能直接判断的事件 CLICK/RELEASED */
		default:
		{

			if(0 != cMouse.button)
				cMouse.event = ConMOUSE_CLICK;
			else
				cMouse.event = ConMOUSE_RELEASED;
		}
		break;
	}/* 完成事件检测 */

	/* 当鼠标滚动时 设置button为滚动值 */
	if(cMouse.event == ConMOUSE_WHEELED || cMouse.event == ConMOUSE_HWHEELED)
	{
		cMouse.button = me.dwButtonState;
	}

return &cMouse;
} 
/* 光标跳转 */ 
void gotoxy( unsigned x, unsigned y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition( hConsoleOut, coord);
}

/* 延迟 */ 
void delay( unsigned int delayMS)
{
	Sleep(delayMS);
}
/* 设置窗口标题 */
bool settitle(const char *title)
{
	return SetConsoleTitle(title);
}
/* 获取窗口标题 */
int gettitle(char *str, int len)
{
	return GetConsoleTitle(str, len);
}
/* 隐藏光标 */
bool hide_cursor()
{
	CONSOLE_CURSOR_INFO cur = { 100, 0};
	return SetConsoleCursorInfo( hConsoleOut, &cur);
}
/* 显示光标 percent 为 0-100 即光标的大小 */ 
bool show_cursor(unsigned int percent)
{
	percent = percent % 101;
	CONSOLE_CURSOR_INFO cur = { percent, true};
	return SetConsoleCursorInfo( hConsoleOut, &cur);
}
/* 设置文字颜色 */
bool set_text_color(unsigned color)
{
	wTextColor = 0;
	if(color & ConRed)
		wTextColor |= FOREGROUND_RED;
	if(color & ConGreen)
		wTextColor |= FOREGROUND_GREEN;
	if(color & ConBlue)
		wTextColor |= FOREGROUND_BLUE;
	
	return SetConsoleTextAttribute(hConsoleOut, wTextColor | wBackgroundColor);
}
/* 设置背景颜色 */
bool set_background_color(unsigned color)
{
	wBackgroundColor = 0;
	if(color & ConRed)
		wBackgroundColor |= BACKGROUND_RED;
	if(color & ConGreen)
		wBackgroundColor |= BACKGROUND_GREEN;
	if(color & ConBlue)
		wBackgroundColor |= BACKGROUND_BLUE;

	return SetConsoleTextAttribute(hConsoleOut, wTextColor | wBackgroundColor);
}
/* 蜂鸣声 */
void beep(int frequence, int duration)
{
	if(beepState)
		Beep(frequence,duration);
}
/* 开关声音 */
void beep_switch(bool turn)
{
	beepState = turn;
}
/* 返回声音的开关状态 */
bool beep_state()
{
	return beepState;
}
char pausei()
{
	return getch();
}

void mouse_mode_active()
{
	DWORD mode;
	GetConsoleMode(hConsoleIn, &mode);
	if(mode & ENABLE_MOUSE_INPUT)
		return;

	SetConsoleMode(hConsoleIn, DEFAULT_CONSOLE_MODE); 
}

void console_test()
{
	DWORD mode;
	GetConsoleMode(hConsoleIn, &mode);
	gotoxy(0, 14);
	printf("Mode is (%d):\n", (int)mode);
#define Tand(t)	if(mode & t)printf(#t "\n");
	Tand(ENABLE_ECHO_INPUT);
	Tand(ENABLE_INSERT_MODE);
	Tand(ENABLE_LINE_INPUT);
	Tand(ENABLE_MOUSE_INPUT);
	Tand(ENABLE_PROCESSED_INPUT);
	Tand(ENABLE_QUICK_EDIT_MODE);
	Tand(ENABLE_WINDOW_INPUT);
}

#endif

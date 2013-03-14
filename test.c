/*
 * Author: Wener
 * E-mail: wenermail@gmail.com
 * Site: http://blog.wener.me
 */
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include "conlib.h"

#define T(c,description) puts(#c);c;puts(description "  按任意键继续");getch();



int main(int argc, char *argv[])
{
	puts("conlib测试程序:");
	T(conlib_init(),"初始化控制台库");
	 
	T(gotoxy(5,5),"现在光标在这里了");
	T(hide_cursor(),"光标不见了"); 
	T(show_cursor(50),"显示一半光标"); 
	T(show_cursor(100),"显示完整光标"); 
	T(settitle("conlib test program"),"设置标题");
	T(set_text_color(ConRed),"设置文字颜色");
	T(set_background_color(ConWhite),"设置背景颜色");
	
	clrscr();
	
	const ConKey* ck;
	const ConMouse* cm;
	const ConControlKeyState *cks; 

	char spaceLine[80] = {[0 ... 78] = ' ',[79] = '\0'};

	

	puts("按键测试:");
	hide_cursor();
#define Tand(v,t)	if(v & t)printf(#t "  ");
#define Teq(v,t)	if(v == t)printf(#t "  ");
	while(1)
	{
		gotoxy(12,0);
		printf("clock: %d\t", (int)clock()); 
		
		ck = getkey();
		cm = getmouse();
		cks = NULL; 
		gotoxy(0,1);
		printf("key value: %p", ck);
		gotoxy(0,2);
		printf("mouse value: %p", cm);
		
		gotoxy(0,3);
		if(ck != NULL)
		{
			cks = ck->ctrl_key_state;
			printf("key code: %d is %s", ck->key, ck->state? "按下": "释放");			
		}
			
		gotoxy(0,4);
		if(cm != NULL)
		{
			cks = cm->ctrl_key_state;
			printf("mouse position: %2d, %2d\n", cm->x, cm->y);
			printf(spaceLine);
			printf(spaceLine);
			printf(spaceLine);
			printf(spaceLine);
			gotoxy(0,5);
			if(cm->event != ConMOUSE_WHEELED && cm->event != ConMOUSE_HWHEELED)
			{
				Tand(cm->button,ConMOUSE_1ST_BUTTON);
				Tand(cm->button,ConMOUSE_2ND_BUTTON);
				Tand(cm->button,ConMOUSE_3RD_BUTTON);
				Tand(cm->button,ConMOUSE_4TH_BUTTON);
				Tand(cm->button,ConMOUSE_RIGHT_BUTTON);
				printf(" 被 按下了\n");
			}else
				printf("鼠标向 [%s] 滚\n", cm->button > 0? "前":"后");
			Teq(cm->event,ConMOUSE_CLICK);
			Teq(cm->event,ConMOUSE_DBCLICK);
			Teq(cm->event,ConMOUSE_MOVED);
			Teq(cm->event,ConMOUSE_WHEELED);
			Teq(cm->event,ConMOUSE_HWHEELED);
			printf(" 被 触发了\n");
		}

		gotoxy(0,10);
#define S(c)	(c? "ON": "OFF")

		if(cks != NULL)
			printf("NUM[%3s]  SCROLL[%3s]  CAPS[%3s]\n"
					"LCTRL[%3s]  LALT[%3s]  RALT[%3s]  RCTRL[%3s]\n"
					"SHIFT[%3s]  ENHANCED[%3s]"
					, S(cks->numlock), S(cks->scrolllock), S(cks->capslock)
					, S(cks->left_ctrl), S(cks->left_alt), S(cks->right_alt), S(cks->right_ctrl)
					, S(cks->shift), S(cks->enhanced));
		console_test();
	}	
	
return EXIT_SUCCESS;
}

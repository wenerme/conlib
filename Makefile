CFLAG=-Wall -g3
conlib:
	gcc -shared -P conlib.c -o conlib_debug.dll $(CFLAG)
	strip conlib_debug.dll -o conlib.dll 
	gcc test.c conlib.dll -o test.exe $(CFLAG)
	gcc beep_test.c conlib.dll -o beep_test


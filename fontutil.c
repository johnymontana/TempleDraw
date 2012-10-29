// fontutil.c	:	font utility functions

// TODO: add support for multiple glut fonts

#include <string.h>


#include <GLUT/glut.h>


#include "fontutil.h"

int fontutil_char_width() {
	return 9;
}

int fontutil_char_height() {
	return 15;
}

int fontutil_string_width(const char* s) {
	return strlen(s) * 10;
}

void fontutil_draw_char(char c) {
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
}

void fontutil_draw_string(const char* s) {
	int i;
	int len = strlen(s);
	for(i = 0; i < len; ++i)
		fontutil_draw_char(s[i]);
}


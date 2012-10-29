#ifndef _fontutil_h_
#define _fontutil_h_

int fontutil_char_width();
int fontutil_char_height();
int fontutil_string_width(const char* s);
void fontutil_draw_char(char c);
void fontutil_draw_string(const char* s);

#endif

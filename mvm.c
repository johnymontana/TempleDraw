// mvm.c	:	draws modelview matrix

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLUT/glut.h>

#include "fontutil.h"

#include "mvm.h"

static GLfloat text_color[3] = {1, 1, 1};
static int mvm_border_x = 10;
static int mvm_border_y = 20;

static int win_w() {return glutGet(GLUT_WINDOW_WIDTH);}
static int win_h() {return glutGet(GLUT_WINDOW_HEIGHT);}

static void mat4_to_string(const double* mat, char** lines) {
	int row, col;
	int i = 0;
	for(row = 0; row < 4; ++row)
		lines[row][0] = '\0';
	for(col = 0; col < 4; ++col) {
		for(row = 0; row < 4; ++row) {
			char num[32];
			sprintf(num, "%6.1f ", mat[i++]);
			strcat(lines[row], num);
		}
	}
};

static void set_ortho_projection() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	GLdouble l = 0;
	GLdouble r = glutGet(GLUT_WINDOW_WIDTH);
	GLdouble b = 0;
	GLdouble t = glutGet(GLUT_WINDOW_HEIGHT);
	GLdouble n = -1;
	GLdouble f = 1;
	glOrtho(l, r, b, t, n, f);
	glMatrixMode(GL_MODELVIEW);
}

static void restore_perspective_projection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void mvm_draw() {
	// get current mv matrix
	double mat[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, mat);

	// setup text drawing
	set_ortho_projection();

	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glLoadIdentity();

	glColor3fv(text_color);

	// draw matrix in upper-left corner
	int x = mvm_border_x; 
	int y = win_h() - mvm_border_y;
	glRasterPos3f(x, y, 0);
	fontutil_draw_string("Modelview Matrix");

	// allocate lines
	char** lines = malloc(sizeof(char*)*4);

	// get matrix lines
	int row, col;
	for(row = 0; row < 4; ++row) {
		lines[row] = malloc(256);
	}
	mat4_to_string(mat, lines);

	// draw lines
	for(row = 0; row < 4; ++row) {
		y -= fontutil_char_height();
		glRasterPos3f(x, y, 0);
		fontutil_draw_string(lines[row]);
	}

	// cleanup
	for(row = 0; row < 4; ++row) {
		free(lines[row]);
	}
	free(lines);

	glPopMatrix();
	glPopAttrib();
	restore_perspective_projection();
}


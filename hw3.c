// hw3.c - homework 3 - temple model
// adapted from manip_teapot sample code project

// William Lyon
// CSCI 441
// HW 3 Assignment
// 10.29.12
// dispalys greek temple model
// To rotate: arrow keys, pageup/pagedown
// To translate: shift + arrow keys, shift + pageup/pagedown

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>


#include <GLUT/glut.h>

#include "precision.h"
#include "geom.h"

#include "mvm.h"

#define FPS 60
#define KEY_ESC 27
#define KEY_j 106
#define KEY_k 107
#define KEY_RESET_TRANSFORM 'r'


// TRIANGLE STRUCT

//struct triangle {
//    float Position[3];
//    float
//};


float verts[] ={
    -2, 2.75, 0,    // 0
    0, 3.5, 0,      // 1
    2, 2.75, 0,     // 2
    -2, 2.5, 0,     // 3
    2, 2.5, 0,      // 4
    0, 3.5, -6,     // 5
    2, 2.75, -6,    // 6
    0, 3.5, -6,     // 7
    -2, 2.75, -6,   // 8
    2, 2.75, -6,    // 9
    2, 2.5, -6,     // 10
    -2, 2.75, -6,   // 11
    -2, 2.5, -6,    // 12
    -1.75, 2.5, 0,  // 13
    -1.5, 2.5, 0,   // 14
    -1.75, 0, 0,    // 15
    -1.5, 0, 0,     // 16
    1.5, 2.5, 0,    // 17
    1.75, 2.5, 0,   // 18
    1.5, 0, 0,      // 19
    1.75, 0, 0,     // 20
    2.5, 0, 0,      // 21
    -2.5, 0, 0,     // 22
    -2.5, -0.25, 0, // 23
    2.5, -0.25, 0,  // 24
    -2.75, -0.25, 1,// 25
    2.75, -0.25, 1, // 26
    -2.75, -0.5, 1, // 27
    2.75, -0.5, 1,  // 28
    -3, -0.5, 2,    // 29
    3, -0.5, 2,     // 30
    -3, -0.75, 2,   // 31
    3, -0.75, 2,    // 32
    -2.5, 0, -6,    // 33
    2.5, 0, -6,     // 34
    -2.5, -0.25, -6,// 35
    2.5, -0.25, -6, // 36
    2.75, -0.25, -7,// 37
    2.75, -0.5, -7, // 38
    -2.75, -0.25, -7,// 39
    -2.75, -0.5, -7,  // 40
    -3, -0.5, -8,     // 41
    3, -0.5, -8,     // 42
    -3, -0.75, -8,    // 43
    3, -0.75, 2,     // 44
    -2, 2.75, -6,   // 45
    0, 3.5, -6,     // 46
    2, 2.75, -6     // 47
};

int tris[] = {
    0, 2, 1,
    3, 2, 0,
    3, 4, 2,
    2, 5, 1,
    5, 2, 6,
    8, 0, 1,
    8, 1, 7,
    4, 9, 2,
    9, 4, 10,
    3, 11, 0,
    11, 3, 12,
    //15, 14, 13,
   // 14, 15, 16,
   // 19, 18, 17,
  //  18, 19, 20,
    23, 21, 22,
    21, 23, 24,
    27, 26, 25,
    26, 27, 28,
    31, 30, 29,
    30, 31, 32,
    22, 21, 34,
    22, 34, 33,
    33, 22, 35,
    23, 22, 35,
    36, 24, 34,
    24, 34, 21,
    35, 34, 36,
    33, 34, 35,
    28, 38, 37,
    28, 37, 26,
    27, 39, 25,
    27, 40, 39,
    40, 39, 37,
    38, 40, 37,
    25, 37, 39,
    25, 26, 37,      // 37
    29, 42, 41,
    29, 30, 42,
    31, 44, 43,
    31, 32, 44,
    43, 29, 41,
    43, 31, 29,
    32, 42, 30,
    32, 44, 42,
    41, 44, 43,
    41, 42, 44,      //47
    46, 47, 45,
    10, 12, 9,
    11, 9, 12
    
};

float normals[1*3] = {
    
};
Real normalVec[] = {0, 0, 0};


void mesh_draw (float* verts, int* tris, int num_tris)
{
	glBegin (GL_TRIANGLES);
	int i;
	for (i=0; i<num_tris; i++)
	{
		//int tri = tri+i*3;
        
		float* v0 = verts + tris[i*3+0]*3;
		float* v1 = verts + tris[i*3+1]*3;
		float* v2 = verts + tris[i*3+2]*3;
        
		geom_calc_normal(v0, v1, v2, normalVec);
        
        glNormal3f(normalVec[0], normalVec[1], normalVec[2]);
        
        glVertex3f(v0[0], v0[1], v0[2]);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
        //glEnd();
        //glFlush();
    }
    
    glEnd();
    glFlush();
}


struct Frame {
	Real basis[3*3];
	Real origin[3];
};
typedef struct Frame Frame;
Frame teapot_frame = {
	{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
	},
	{ 0, 0, 0},
};



// window
static const char* win_title = "Greek Temple";
static int win_id = 0;
static int init_win_width = 800;
static int init_win_height = 600;
static int win_w() {return glutGet(GLUT_WINDOW_WIDTH);}
static int win_h() {return glutGet(GLUT_WINDOW_HEIGHT);}

// camera
static Real eye[] = {4, 2, 6};
static Real cen[] = {0, 0, 0};
static Real up[] = {0, 1, 0};

// input
static Real rot_angle = 5.0;
static Real trans_dist = 0.1;

// tick rate
static double frame_time = 1.0 / FPS;
static double prev_tick_sec = 0;

// fps
static int frame_count = 0;
static double prev_fps_sec = 0;
static int prev_fps_frame = 0;
static double fps_update_interval = 1.0;

// teapot
static Real teapot_color[3] = {0.0, 0.5, 0.7};

static void reset_frame();

// ===== util

// returns current time in seconds
static double get_time() {
	struct timeval now;
	gettimeofday(&now, NULL);
	double now_sec = now.tv_sec + now.tv_usec * 1e-6;
	return now_sec;
}

// ===== input
static void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case KEY_ESC:
		exit(0);

	case KEY_RESET_TRANSFORM:
		reset_frame();
		break;
            
        

	default:
		break;
	}
}

static void reset_frame() {
	Frame* frm = &teapot_frame;
	geom_matrix3_new_ident(frm->basis);
	geom_vector3_zero(frm->origin);
}

static void translate_x(float dist) {
	Frame* frm = &teapot_frame;
	Real vtrans[3] = {dist, 0, 0};
	geom_vector3_add(frm->origin, vtrans, frm->origin);
}

static void translate_y(float dist) {
	Frame* frm = &teapot_frame;
	Real vtrans[3] = {0, dist, 0};
	geom_vector3_add(frm->origin, vtrans, frm->origin);
}

static void translate_z(float dist) {
	Frame* frm = &teapot_frame;
	Real vtrans[3] = {0, 0, dist};
	geom_vector3_add(frm->origin, vtrans, frm->origin);
}

static void rotate_x(float angle) {
	Frame* frm = &teapot_frame;
	Real mat[3*3];
	Real vectorToRotate[3];
    vectorToRotate[0]=frm->basis[0];
    vectorToRotate[1]=frm->basis[1];
    vectorToRotate[2]=frm->basis[2];
    
    //geom_matrix3_new_rotx(angle, mat);
    //int geom_matrix3_new_rot(const Real* vrot, Real angle_deg, Real* mat_rot_ret)
    geom_matrix3_new_rot(vectorToRotate, angle, mat);
	geom_matrix3_mul(frm->basis, mat, frm->basis);
}

static void rotate_y(float angle) {
	Frame* frm = &teapot_frame;
	Real mat[3*3];
    Real vectorToRotate[3];
    vectorToRotate[0]=frm->basis[3];
    vectorToRotate[1]=frm->basis[4];
    vectorToRotate[2]=frm->basis[5];
	//geom_matrix3_new_roty(angle, mat);
	geom_matrix3_new_rot(vectorToRotate, angle, mat);
    geom_matrix3_mul(frm->basis, mat, frm->basis);
}

static void rotate_z(float angle) {
	Frame* frm = &teapot_frame;
	Real mat[3*3];
	Real vectorToRotate[3];
    vectorToRotate[0]=frm->basis[6];
    vectorToRotate[1]=frm->basis[7];
    vectorToRotate[2]=frm->basis[8];
    geom_matrix3_new_rot(vectorToRotate, angle, mat);
    //geom_matrix3_new_rotz(angle, mat);
	geom_matrix3_mul(frm->basis, mat, frm->basis);
}

static void special(int key, int x, int y) {
	int translate = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

	float dir;

	switch(key) {
	case KEY_ESC:
		exit(0);

	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		dir = key == GLUT_KEY_UP ? 1 : -1;
		if(translate)
			translate_y(trans_dist*dir);
		else
			rotate_x(-rot_angle*dir);
		break;
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		dir = key == GLUT_KEY_RIGHT ? 1 : -1;
		if(translate)
			translate_x(trans_dist*dir);
		else
			rotate_y(rot_angle*dir);
		break;
            
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            dir = key == GLUT_KEY_PAGE_UP ? 1 : -1;
            if(translate)
                translate_z(trans_dist*dir);
            else
                rotate_z(-rot_angle*dir);
            break;

	default:
		break;
	}
}

// ===== drawing
static void draw_axes() {
	glDisable(GL_LIGHTING);
	Real axis_length = 5;
	int i;
	for(i = 0; i < 3; ++i) {
		Real axis[3] = {i==0?1:0, i==1?1:0, i==2?1:0};
		glColor3fv(axis);
		glBegin(GL_LINES);
			Real v[3];
			geom_vector3_scalar_mul(axis, axis_length, v);
			glVertex3fv(v);
			geom_vector3_scalar_mul(axis, -axis_length, v);
			glVertex3fv(v);
		glEnd();
	}
	glEnable(GL_LIGHTING);
}



static void draw_temple() {
	glColor3fv(teapot_color);
	   
    mesh_draw(verts, tris, 46);
    glPushMatrix();
    GLUquadricObj * qobj = gluNewQuadric();
    
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glTranslatef(-1.75, 0, 0);
    glTranslatef(0, 0, -2.5);
    glTranslatef(0, -0.3, 0);
    

    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(1.15, 0, 0);
    
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    
    glTranslatef(1.15, 0, 0);
    
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, -1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    
    glTranslatef(1.15, 0, 0);
    
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);
    glTranslatef(0, 1.75, 0);
    gluCylinder(qobj, 0.25, 0.25, 2.5, 34, 34);

    
    gluDeleteQuadric(qobj);
    glPopMatrix();
    
    }

static void apply_frame() {
	Frame* frm = &teapot_frame;
	Real mat[16];
	mat[ 0]=frm->basis[0]; mat[ 1]=frm->basis[1]; mat[ 2]=frm->basis[2]; mat[ 3]=0;
	mat[ 4]=frm->basis[3]; mat[ 5]=frm->basis[4]; mat[ 6]=frm->basis[5]; mat[ 7]=0;
	mat[ 8]=frm->basis[6]; mat[ 9]=frm->basis[7]; mat[10]=frm->basis[8]; mat[11]=0;
	mat[12]=frm->origin[0]; mat[13]=frm->origin[1]; mat[14]=frm->origin[2]; mat[15]=1;
	glMultMatrixf(mat);
    
}

static void check_gl_errors() {
    GLuint err;
    while((err=glGetError())!=GL_NO_ERROR) {
        printf("GL ERROR: %s\n", gluErrorString(err));
    }
}

static void set_more_lighting() {
    float pos[4] = {10, 10, 10, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    
}
static void display() {

	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set camera viewpoint
	gluLookAt(
		eye[0], eye[1], eye[2],
		cen[0], cen[1], cen[2],
		up[0], up[1], up[2]);
    
    set_more_lighting();
    
	draw_axes();

	glPushMatrix();

		// draw temple
		apply_frame();
		draw_temple();

		// draw teapot matrix transform
		glPushMatrix();
			glLoadIdentity();	 	// remove effect of camera transform
			apply_frame();
			mvm_draw();
		glPopMatrix();

	glPopMatrix();

	glFlush();
	glutSwapBuffers();

	// increment frame count for framerate code
	++frame_count;
    check_gl_errors();
    
}

static void set_projection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float fov = 90;
	float aspect = win_w()/win_h();
	float n = 0.1;
	float f = 100;
	gluPerspective(fov, aspect, n, f);
	glMatrixMode(GL_MODELVIEW);
}

static void set_lighting() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
}

// ===== update simulation state
static void tick(float dt) {
	// we don't do anything here
}

static void idle() {
	// tick sim every frame
	// call game tick every frame interval
	double now_sec = get_time();
	double diff_tick_sec = now_sec - prev_tick_sec;
	if(diff_tick_sec > frame_time) {
		tick(frame_time);
		prev_tick_sec = now_sec;
		glutPostRedisplay();
	}

	// calc fps every fps update interval
	double diff_fps_sec = now_sec - prev_fps_sec;
	if(diff_fps_sec > fps_update_interval) {
		int diff_frames = frame_count - prev_fps_frame;
		double fps = diff_frames / diff_fps_sec;
		char title[256];
		sprintf(title, "%s - %d fps", win_title, (int)fps);
		glutSetWindowTitle(title);
		glutPostRedisplay();
		prev_fps_sec = now_sec;
		prev_fps_frame = frame_count;
	}
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(init_win_width, init_win_height);
	win_id = glutCreateWindow(win_title);
	if(win_id <= 0) {
		fprintf(stderr, "Error: glutCreateWindow() returned %d\n", win_id);
		exit(1);
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutIdleFunc(idle);

	set_projection();
	set_lighting();
	
	glutMainLoop();

	return 0;
}

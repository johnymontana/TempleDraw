#ifndef _precision_h_
#define _precision_h_

#ifdef SINGLE_PRECISION
typedef float Real;
#define glVertex3r glVertex3f
#define glVertex3rv glVertex3fv
#define glNormal3rv glNormal3fv
#define glTexCoord2r glTexCoord2f
#define glColor3rv glColor3fv
#define glColor4r glColor4f
#define glColor4rv glColor4fv
#define glMultMatrixr glMultMatrixf
#define glGetRealv glGetFloatv

#elif DOUBLE_PRECISION
typedef double Real;
#define glVertex3r glVertex3d
#define glVertex3rv glVertex3dv
#define glNormal3rv glNormal3dv
#define glTexCoord2r glTexCoord2d
#define glColor3rv glColor3dv
#define glColor4r glColor4d
#define glColor4rv glColor4dv
#define glMultMatrixr glMultMatrixd
#define glGetRealv glGetDoublev

//#else
//#error "either SINGLE_PRECISION or DOUBLE_PRECISION must be defined"
#endif

void precision_print();
#endif

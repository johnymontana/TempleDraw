uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
#CFLAGS += -I.
#Linux (default)
#EXE = hw3
LDFLAGS = -lGL -lGLU -lglut
	
#OS X
ifeq ($(uname_S), Darwin)
	LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
endif

#$(EXE) : geom.c fontutil.c precision.c mvm.c manip_teapot.c
#	 gcc -Wall -o $@ $< $(CFLAGS) $(LDFLAGS)

hw3: geom.c fontutil.c precision.c mvm.c manip_teapot.c
	gcc -o hw3 geom.c fontutil.c precision.c mvm.c hw3.c -I. $(LDFLAGS)

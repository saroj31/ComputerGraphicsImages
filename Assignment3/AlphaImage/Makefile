CC		= g++
C		= cpp

CFLAGS		= -g
LFLAGS		= -g

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS   = -L /usr/lib64/ -lglut -lGL -lGLU -lOpenImageIO -lm
  endif
endif

PROJECT		= Alphamask

${PROJECT}:	${PROJECT}.o RGBtoHSV.o GLView.o OIIOServices.o ImageData.o
	${CC} ${LFLAGS} -o ${PROJECT} ${PROJECT}.o RGBtoHSV.o GLView.o OIIOServices.o ImageData.${C} ${LDFLAGS}

${PROJECT}.o:	${PROJECT}.${C} RGBtoHSV.${C} GLView.${C} OIIOServices.${C} ImageData.${C}
	${CC} ${CFLAGS} -c ${PROJECT}.${C}

clean:
	rm -f core.* *.o *~ ${PROJECT}

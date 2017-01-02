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

PROJECT		= imgview

${PROJECT}:	${PROJECT}.o GLServices.o OIIOServices.o
	${CC} ${LFLAGS} -o ${PROJECT} ${PROJECT}.o GLServices.o OIIOServices.o ${LDFLAGS}

${PROJECT}.o:	${PROJECT}.${C} GLServices.${C} OIIOServices.${C}
	${CC} ${CFLAGS} -c ${PROJECT}.${C}

clean:
	rm -f core.* *.o *~ ${PROJECT}

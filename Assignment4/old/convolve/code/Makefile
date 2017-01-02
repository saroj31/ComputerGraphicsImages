CC		= g++
C		= cpp

CFLAGS		= -g -Wall `Magick++-config --cppflags`
LFLAGS		= -g `Magick++-config --ldflags`

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lMagick++ -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -L /usr/lib64/ -lglut -lGL -lMagick++ -lm
  endif
endif

HFILES = FloatImage.h Kernel.h
OBJS =	FloatImage.o Kernel.o

PROJECT		= filt

$(PROJECT) : $(OBJS) $(PROJECT).o
	${CC} ${LFLAGS} -o $(PROJECT) $(OBJS) $(PROJECT).o $(LDFLAGS)
	cp $(PROJECT) ..

$(PROJECT).o    : $(PROJECT).cpp $(HFILES)
	${CC} ${CFLAGS} -c $(PROJECT).cpp 

#nfilt :  $(OBJS) nfilt.o
#	${CC} ${CFLAGS} -o nfilt $(OBJS) nfilt.o $(LDFLAGS) 

#nfilt.o    : nfilt.cpp ppmrw.h floatimage.h
#	${CC} ${CFLAGS} -c nfilt.cpp 

#median :  $(OBJS) median.o
#	${CC} ${CFLAGS} -o median $(OBJS) median.o $(LDFLAGS) 

#median.o    : median.cpp ppmrw.h floatimage.h
#	${CC} ${CFLAGS} -c median.cpp 

#makeimages: makeimages.cpp floatimage.o ppmrw.o
#	${CC} ${CFLAGS} -o makeimages makeimages.cpp floatimage.o ppmrw.o -lgl -lm

FloatImage.o : FloatImage.cpp FloatImage.h
	${CC} ${CFLAGS} -c FloatImage.cpp
	
Kernel.o : Kernel.cpp Kernel.h
	${CC} ${CFLAGS} -c Kernel.cpp 

clean:
	rm -f core.* *.o *~ ${PROJECT}

include main_objects.h
#
PROGRAM = mvs10
#
CC = g++
CFLAGS = -g -Wall\
-I.. -I../../common
#
all: $(PROGRAM)
#
$(PROGRAM): $(main_objects) ../mvs10.o
	$(CC) -o $@ $(main_objects)\
        ../mvs10.o\
        ../../dmag5/dmag5.o\
        ../../er9b/er9b.o\
        ../../sfm10/sfm10.o\
        ../../common/io/io.o\
        ../../common/jpeg/jpeg.o\
        ../../common/math/math.o\
        ../../common/png/png.o\
        ../../common/tiff/tiff.o\
        ../../common/util/util.o\
        ../../sfm10/liblbfgs/lib/liblbfgs.a\
        ../../sfm10/pba/bin/libpba_no_gpu.a\
        -pthread\
        -ltiff\
        -lpng\
        -ljpeg\
        -lz\
        -lm
#
.cpp.o:
	$(CC) -c $(CFLAGS) $<

all: mosaic web
CC=g++
mosaic : solution.cpp ass2.o params.h
	$(CC) solution.cpp ass2.o -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -o mosaic.out

web : web.cpp ass2.o params.h
	$(CC) web.cpp ass2.o -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_nonfree -lopencv_flann -lopencv_features2d -o web.out

ass2.o : ass2.h ass2.cpp
	$(CC) -c ass2.cpp -o ass2.o

rmimage : 
	rm *_temp.jpg

clean : 
	rm -f ass2.o mosaic.out

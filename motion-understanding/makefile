all: solution.out
CC=g++ -g
solution.out : solution.cpp ass4.o params.h
	$(CC) solution.cpp ass4.o -lm -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -o solution.out

ass4.o : ass4.h ass4.cpp params.h
	$(CC) -c ass4.cpp -o ass4.o

clean : 
	rm -f ass4.o solution.out

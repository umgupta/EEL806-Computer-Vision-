all: morph.out

morph.out : solution.cpp ass1.o params.h
	g++ solution.cpp ass1.o -lopencv_core -lopencv_imgproc -lopencv_highgui -o morph.out

ass1.o : ass1.h ass1.cpp
	g++ -c ass1.cpp -o ass1.o

rmimage : 
	rm *_save.jpg

clean : 
	rm ass1.o morph.out

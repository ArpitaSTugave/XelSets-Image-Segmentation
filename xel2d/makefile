UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
GL_LINK = -lglut -lGL -lGLU
endif
ifeq ($(UNAME), Darwin)
GL_LINK = -framework OpenGL -framework GLUT 
endif

xel2d.out: xel2dmain.o uihandler2d.o xel2d.o param2d.o input2d.o point2d.o xeltree2d.o
	g++  -o xel2d.out obj/xel2dmain.o obj/uihandler2d.o obj/xel2d.o obj/param2d.o obj/input2d.o obj/point2d.o obj/xeltree2d.o `pkg-config --libs opencv` $(GL_LINK)     

param2d.o: src/param2d.cpp src/param2d.h
	g++ -c `pkg-config --cflags opencv` -o obj/`basename param2d.o` src/param2d.cpp 
	
point2d.o: src/point2d.cpp src/point2d.h 
	g++ -c src/point2d.cpp -o obj/point2d.o
	
xel2d.o: src/xel2d.cpp src/xel2d.h src/param2d.h src/point2d.h src/xeltree2d.h
	g++ -c src/xel2d.cpp -o obj/xel2d.o
	
xeltree2d.o: src/xeltree2d.cpp src/xeltree2d.h src/param2d.h src/point2d.h
	g++ -c src/xeltree2d.cpp -o obj/xeltree2d.o
	
input2d.o: src/input2d.cpp src/input2d.h src/param2d.h src/xel2d.h 
	g++ -c `pkg-config --cflags opencv` -o obj/`basename input2d.o` src/input2d.cpp 
	
uihandler2d.o: src/uihandler2d.cpp src/uihandler2d.h src/param2d.h src/xel2d.h src/input2d.h
	g++ -c src/uihandler2d.cpp -o obj/uihandler2d.o -D$(UNAME)
	
xel2dmain.o: src/xel2dmain.cpp src/uihandler2d.h src/xel2d.h src/param2d.h src/input2d.h
	g++ -c src/xel2dmain.cpp -o obj/xel2dmain.o
	

	

		

	

clean:
	rm obj/*.o xel2d.out



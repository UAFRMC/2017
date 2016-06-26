HEADERS=fox_ui.h fox_motor.h
UTIL=serial.cpp

OPTS=-O3 -ffast-math -fopenmp

DIR=libfreenect
L=$(DIR)/lib
LINK=-Wl,-rpath=$L -L$L -lfreenect 
INC=-I$(DIR)/include -I../webdemo/ -I/usr/local/include/libfreenect


all: ncurses

ncurses: ncurses.cpp $(HEADERS)
	g++ $(OPTS)  -o $@   $<  $(UTIL) -lncurses

kinect: kinect.cpp $(HEADERS)
	g++ $(OPTS)  -o $@  $(INC) $<  $(UTIL) $(LINK) -lncurses -lglut

clean:
	rm main *.o

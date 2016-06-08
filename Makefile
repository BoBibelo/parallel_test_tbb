CPPFLAGS=
CXX=clang++ -pthread
CXXFLAGS= -Wall -Wextra -std=c++14 -O3
LDFLAGS=
LDLIBS= -lrt -ltbb

mandelbrot: CXXFLAGS+=`sdl-config --cflags`
mandelbrot: LDLIBS+=`sdl-config --libs`

all:

clean:
	rm -f *.o *~

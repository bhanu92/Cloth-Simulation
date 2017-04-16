PROG = simulation

CC = g++
CFLAGS = -g -w -D_DEBUG -std=c++11
LIBS = -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -lGLEW -lSOIL
OBJS = Cloth.o Main.o Rotator.o LoadShaders.o Texture.o

all: $(PROG)

gl3w.o: gl3w.c
	gcc -c $(CFLAGS) -o $@ $<

.cpp.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -g -o $@ $^ $(LIBS)

clean:
	rm -f *.o $(PROG) *.gch

CPP = g++
CFLAGS = -c -g
HEAD = jakmuse_common.h jakmuse_sound.hpp
OBJS = jakmuse.o jakmuse_parser.o jakmuse_sound.o
EXENAME = jakmuse

$(EXENAME): $(HEAD) $(OBJS)
	g++ -o $(EXENAME) $(OBJS) -lm -lSDL -lrt

%.o: %.cpp
	g++ -o $@ $(CFLAGS) $<

clean:
	rm -f jakmuse *.o

VERSION = "2.0"
CPP = g++
CFLAGS = -c -g --std=gnu++11 -DVERSION=$(VERSION)
HEAD = jakmuse_common.h jakmuse_sound.hpp
OBJS = jakmuse.o jakmuse_parser.o jakmuse_sound.o jakmuse_generators.o jakmuse_wave.o
EXENAME = jakmuse

$(EXENAME): $(HEAD) $(OBJS)
	g++ -o $(EXENAME) $(OBJS) -lm -lSDL -lrt

%.o: %.cpp
	g++ -o $@ $(CFLAGS) $<

clean:
	rm -f jakmuse *.o

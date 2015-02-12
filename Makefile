VERSION = "\"2.0\""
CC = g++
CFLAGS = -c -gsplit-dwarf --std=gnu++11 -DVERSION=$(VERSION) -Wno-multichar -O3 -msse -msse2 -msse3 -mssse3 -fopenmp
HEAD = jakmuse_common.h
OBJS = jakmuse.o jakmuse_parser.o jakmuse_sound.o jakmuse_generators.o jakmuse_wave.o
EXENAME = jakmuse

$(EXENAME): $(HEAD) $(OBJS)
	$(CC) -gsplit-dwarf -o $(EXENAME) $(OBJS) -lm -lSDL -lrt -lgomp

%.o: %.cpp
	$(CC) $(CFLAGS) $<

clean:
	rm -f jakmuse *.o *.dwo

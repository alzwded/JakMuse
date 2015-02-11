CC = cl.exe
SDLROOT = SDL-1.2.15
CFLAGS = /Ot /c /EHsc /I"$(SDLROOT)\include"
LD = link.exe
PLA = x86
LDOPTS = /OUT:jakmuse.exe /LIBPATH:"$(SDLROOT)\lib\$(PLA)"
LIBS = SDL.lib

.SUFFIXES:.cpp .hpp .h .obj

OBJS = jakmuse.obj jakmuse_parser.obj jakmuse_sound.obj jakmuse_generators.obj

jakmuse.exe: $(OBJS) SDL.dll
	$(LD) $(LDOPTS) $(OBJS) $(LIBS)

SDL.dll:
	copy $(SDLROOT)\lib\$(PLA)\SDL.dll .

{..\}.cpp{}.obj::
	$(CC) $(CFLAGS) $<

clean:
	del /s *.obj jakmuse.exe

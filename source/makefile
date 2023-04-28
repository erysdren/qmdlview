
CFLAGS += -Wall -pedantic -O2

SOURCES = qmdlview.c mdl.c

SDL2 = platform_sdl2_renderer.c `sdl2-config --libs --cflags`

all: clean qmdlview

qmdlview: $(SOURCES)
	$(CC) -o qmdlview $(SOURCES) $(SDL2) $(CFLAGS)

clean:
	$(RM) qmdlview *.exe

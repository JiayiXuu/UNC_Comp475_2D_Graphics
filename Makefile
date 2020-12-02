# define CPPFLAGS=-I... for other (system) includes
# define LDFLAGS=-L... for other (system) libs to link

CC = g++ -g -Wno-float-conversion -Wno-narrowing -Wreturn-type -Wunused-function -Wreorder -Wunused-variable

CC_DEBUG = @$(CC) -std=c++11
CC_RELEASE = @$(CC) -std=c++11 -O3 -DNDEBUG

G_SRC = src/*.cpp *.cpp
G_DEPS = *.cpp Makefile

G_INC = -Iinclude -Iapps $(CPPFLAGS)

G_LINK = $(LDFLAGS)

all: image tests bench

image : $(G_SRC) $(G_DEPS) apps/image.cpp apps/image_recs.cpp
	$(CC_DEBUG) $(G_INC) $(G_SRC) apps/image.cpp apps/image_recs.cpp -o image

tests : $(G_SRC) $(G_DEPS) apps/tests.cpp apps/tests_recs.cpp
	$(CC_DEBUG) $(G_INC) $(G_SRC) apps/tests.cpp apps/tests_recs.cpp -o tests

bench : $(G_SRC) $(G_DEPS) apps/bench.cpp apps/bench_recs.cpp apps/GTime.cpp
	$(CC_RELEASE) $(G_INC) $(G_SRC) apps/GTime.cpp apps/bench.cpp apps/bench_recs.cpp -o bench

# debug variant of bench -- not any good for timing, but helps debugging --once
dbench : $(G_SRC) $(G_DEPS) apps/bench.cpp apps/bench_recs.cpp apps/GTime.cpp
	$(CC_DEBUG) $(G_INC) $(G_SRC) apps/GTime.cpp apps/bench.cpp apps/bench_recs.cpp -o dbench

DRAW_SRC = apps/draw.cpp apps/GWindow.cpp apps/GTime.cpp
draw: $(DRAW_SRC) $(G_SRC) $(G_DEPS)
	$(CC_RELEASE) $(G_INC) $(G_SRC) $(DRAW_SRC) $(G_LINK) -lSDL2 -o draw

PAINT_SRC = apps/paint.cpp apps/GWindow.cpp apps/GTime.cpp
paint: $(PAINT_SRC) $(G_SRC) $(G_DEPS)
	$(CC_RELEASE) $(G_INC) $(G_SRC) $(PAINT_SRC) $(G_LINK) -lSDL2 -o paint

VIEWER_SRC = apps/viewer.cpp apps/GWindow.cpp apps/GTime.cpp apps/image_recs.cpp
viewer: $(VIEWER_SRC) $(G_SRC) $(G_DEPS)
	$(CC_RELEASE) $(G_INC) $(G_SRC) $(VIEWER_SRC) $(G_LINK) -lSDL2 -o viewer


BOUNCE_SRC = apps/bounce.cpp apps/GWindow.cpp apps/GTime.cpp
bounce: $(BOUNCE_SRC) $(G_SRC) $(G_DEPS)
	$(CC_RELEASE) $(G_INC) $(G_SRC) $(BOUNCE_SRC) $(G_LINK) -lSDL2 -o bounce


clean:
	@rm -rf image tests draw paint viewer bounce bench dbench *.png *.dSYM


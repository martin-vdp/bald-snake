all:
	gcc main.c `sdl2-config --cflags --libs` -O -lSDL2_image -lSDL2_ttf -lSDL2_mixer -Wall -Werror && ./a.out

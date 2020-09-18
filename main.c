#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char **argv)
{

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Couldn't init video: %s", SDL_GetError());
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Bald Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0); 

	if(window == NULL)
		fprintf(stderr, "SDL error: %s", SDL_GetError());

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	SDL_Event event;
	int running = 1;
	
	while(running)
	{

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:

					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = 0;
							break;
					
					}

					break;
				case SDL_QUIT:
					running = 0;
					break;
			
			}
		}
		
		

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);



		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

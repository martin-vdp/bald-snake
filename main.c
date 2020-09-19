#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char **argv)
{

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Couldn't init video: %s", SDL_GetError());
		return -1;
	}

	if(TTF_Init() != 0)
	{
		fprintf(stderr, "Couldn't init TTF: %s", TTF_GetError());
		return -2;
	}
	
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf(stderr, "Couldn't init IMG: %s", IMG_GetError());
		return -3;
	}

	SDL_Window* window = SDL_CreateWindow("Bald Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0); 

	if(window == NULL)
		fprintf(stderr, "SDL error: %s", SDL_GetError());

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	

	SDL_Event event;
	
	unsigned int isInMenu = 1;

	unsigned int placeInMenu = 0;


	SDL_Color white = {255, 255, 255};
	SDL_Color red   = {255, 0, 0};
	

	TTF_Font* font = TTF_OpenFont("assets/fonts/Roboto-Black.ttf", 1000);

	SDL_Surface* surface_playtext = TTF_RenderText_Solid(font, "Play", red);
	SDL_Texture* texture_playtext = SDL_CreateTextureFromSurface(renderer, surface_playtext);

	SDL_Surface* surface_quittext = TTF_RenderText_Solid(font, "Quit", white);
	SDL_Texture* texture_quittext = SDL_CreateTextureFromSurface(renderer, surface_quittext);

	SDL_Surface* surface_banner = IMG_Load("assets/img/game_banner.png");
	if(!surface_banner)
	{
		fprintf(stderr, "Couldn't load the image: %s\n", IMG_GetError());
		return -4;
	}
	SDL_Texture* texture_banner = SDL_CreateTextureFromSurface(renderer, surface_banner);

	SDL_Rect play_rect;
	play_rect.x = 400; 	
	play_rect.y = 325;
	play_rect.w = 200;
	play_rect.h = 200;

	SDL_Rect quit_rect;
	quit_rect.x = 400; 	
	quit_rect.y = 500;
	quit_rect.w = 200;
	quit_rect.h = 200;

	SDL_Rect banner_image_rect;
	banner_image_rect.x = 250;
	banner_image_rect.y = 0;
	banner_image_rect.w = 500;
	banner_image_rect.h = 300;


	// rendering menu
	while(isInMenu)
	{
		SDL_Delay(50);
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
					case SDLK_UP:
						if(placeInMenu == 0) break;
						placeInMenu--;
						surface_playtext = TTF_RenderText_Solid(font, "Play", red);
						texture_playtext = SDL_CreateTextureFromSurface(renderer, surface_playtext);

						surface_quittext = TTF_RenderText_Solid(font, "Quit", white);
						texture_quittext = SDL_CreateTextureFromSurface(renderer, surface_quittext);
	

						break;

					case SDLK_DOWN:
						if(placeInMenu == 1) break;
						placeInMenu++;
						
						surface_playtext = TTF_RenderText_Solid(font, "Play", white);
						texture_playtext = SDL_CreateTextureFromSurface(renderer, surface_playtext);

						surface_quittext = TTF_RenderText_Solid(font, "Quit", red);
						texture_quittext = SDL_CreateTextureFromSurface(renderer, surface_quittext);

						break;

					case SDLK_RETURN: case SDLK_KP_ENTER:
						// if it's the play button clear the menu resources if not just exit :)
						if(placeInMenu == 0) {isInMenu = 0; SDL_FreeSurface(surface_playtext); SDL_FreeSurface(surface_quittext); SDL_FreeSurface(surface_banner);
						SDL_DestroyTexture(texture_banner); SDL_DestroyTexture(texture_playtext); SDL_DestroyTexture(texture_quittext); printf("DICK\n");  break;}
						else exit(0);


					case SDLK_ESCAPE:
						exit(0);
						break;
					
					}
					break;
				case SDL_QUIT:
					exit(0);
					break;
	
			
		}

		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		SDL_RenderCopy(renderer, texture_banner, NULL, &banner_image_rect);
		SDL_RenderCopy(renderer, texture_playtext, NULL, &play_rect);
		SDL_RenderCopy(renderer, texture_quittext, NULL, &quit_rect);
		
		SDL_RenderPresent(renderer);
	
	}
	

	
	unsigned int running = 1;
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

	TTF_Quit();
	IMG_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

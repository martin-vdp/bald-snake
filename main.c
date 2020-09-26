#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

//
// TODO:
// 	- Changing the struct Tail and the struct Food with a typedef so we can save 8 bytes
// 	  (it needs to be able to run in a freaking toaster).
//


struct Tail
{
	unsigned int x, y;
};

enum direction
{
	UP    = 1,
	DOWN  = 2,
	LEFT  = 3,
	RIGHT = 4,
};

struct Snake
{
	unsigned int x, y;
	unsigned int next_direction;
	unsigned int total;
	struct Tail* tail;
};

void snake_init(struct Snake* snake)
{
	snake->x = 10;
	snake->y = 10;
	snake->next_direction = RIGHT;
	snake->total = 0;
	snake->tail = malloc(sizeof(struct Tail)*0); // making it null at the beginning maybe ?
}

struct Food
{
	unsigned int x,y;
};

unsigned int food_is_in_snake(struct Food food, struct Snake snake)
{
	for(unsigned int i=0;i<snake.total;i++)
	{
		if(snake.tail[i].x == food.x && snake.tail[i].y == food.y)
			return 1;
	}
	return 0;
}

void random_food(struct Food* food, struct Snake snake)
{
	srand(time(NULL));

	struct Food last_food = *food;	

	
	
	do
	{
	food->x = (rand() % 20);
	food->y = (rand() % 20);
	}while((food->x == last_food.x && food->y == last_food.y) || food_is_in_snake(*food, snake) );
}

void snake_render(SDL_Renderer* renderer, struct Snake snake, SDL_Texture* head_texture)
{
	SDL_SetRenderDrawColor(renderer, 51, 255, 51, 0);

	SDL_Rect snake_rect;
	snake_rect.x = (snake.x * 50);
	snake_rect.y = (snake.y * 50);
	snake_rect.w = 50;
	snake_rect.h = 50;

	SDL_RenderFillRect(renderer, &snake_rect);

	SDL_RenderCopy(renderer, head_texture, NULL, &snake_rect);


	for(unsigned int i=0;i<snake.total;i++)
	{
		snake_rect.x = (snake.tail[i].x * 50);
		snake_rect.y = (snake.tail[i].y * 50);
		SDL_RenderFillRect(renderer, &snake_rect);
	}
}

void game_update(struct Snake* snake, struct Food* food, SDL_Rect* food_rect)
{
	struct Snake snake_last_position = *snake;

	switch(snake->next_direction)
	{
		case UP:
			snake->y--;
			break;
		case DOWN:
			snake->y++;
			break;
		case LEFT:
			snake->x--;
			break;
		case RIGHT:
			snake->x++;
			break;	
	}

	if(19 < snake->x || 19 < snake->y || snake->x < 0 || snake->y < 0)
	{
		free(snake->tail);
		snake_init(snake);
	}

	for(unsigned int i=0; i<snake->total;i++)
	{
		if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y)
		{
			free(snake->tail);
			snake_init(snake);
		}
	}

	if((snake->x == food->x) && (snake->y == food->y))
	{
		random_food(food, *snake);
		food_rect->x = food->x * 50;
		food_rect->y = food->y * 50;

		snake->total++;

		struct Tail new_tail = {snake->x, snake->y};
		snake->tail = realloc(snake->tail, sizeof(struct Tail) * snake->total);
		snake->tail[snake->total-1] = new_tail;
	}
	


	for (int i = 0; i < snake->total; i++)
	{
		if (i > 0)
		{
			snake->tail[i-1].x = snake->tail[i].x;
			snake->tail[i-1].y = snake->tail[i].y;
		}

	}

	if (snake->total > 0)
	{
		snake->tail[snake->total-1].x = snake_last_position.x;			
		snake->tail[snake->total-1].y = snake_last_position.y;
	}


//	for()


}

void render_grid(SDL_Renderer* renderer)
{

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	for(unsigned int i=1;i<20;i++)
	{
		SDL_RenderDrawLine(renderer, i*50, 0, i*50, 1000);
		SDL_RenderDrawLine(renderer, 0, i*50, 1000, i*50);
	}
}

int main(int argc, char **argv)
{

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		fprintf(stderr, "Couldn't init video or / and audio: %s", SDL_GetError());
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
	SDL_Color green  = {0, 255, 0};

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

	SDL_Surface* surface_footertext = TTF_RenderText_Solid(font, "don't judge pls", green);
	SDL_Texture* texture_footertext = SDL_CreateTextureFromSurface(renderer, surface_footertext);

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
	banner_image_rect.y = 20;
	banner_image_rect.w = 500;
	banner_image_rect.h = 300;

	SDL_Rect footertext_rect;
	footertext_rect.x = 120;
	footertext_rect.y = 625;
	footertext_rect.w = 750;
	footertext_rect.h = 380;

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
						if(placeInMenu == 0) {isInMenu = 0; SDL_FreeSurface(surface_playtext); SDL_FreeSurface(surface_quittext); SDL_FreeSurface(surface_banner); SDL_FreeSurface(surface_footertext);
						SDL_DestroyTexture(texture_footertext);SDL_DestroyTexture(texture_banner); SDL_DestroyTexture(texture_playtext); SDL_DestroyTexture(texture_quittext); break;}
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
		SDL_RenderCopy(renderer, texture_footertext, NULL, &footertext_rect);

		SDL_RenderPresent(renderer);
	
	}
	

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	Mix_Music* ingame_music = Mix_LoadMUS("assets/music/temporary_music.wav");
	if(!ingame_music)
	{
		fprintf(stderr, "couldn't open music: %s\n", Mix_GetError());
		return -1;
	}

	Mix_PlayMusic(ingame_music, -1);

	SDL_Surface* head_img_surface = IMG_Load("assets/img/head.png");
	if(!head_img_surface)
	{
		fprintf(stderr, "Couldn't load head pic: %s\n", IMG_GetError());
		return -1;
	}
	SDL_Texture* head_img_texture = SDL_CreateTextureFromSurface(renderer, head_img_surface);


	SDL_Surface* food_img_surface = IMG_Load("assets/img/food.png");
	if(!food_img_surface)
	{
		fprintf(stderr, "Couldn't load food pic: %s\n", IMG_GetError());
	}
	SDL_Texture* food_img_texture = SDL_CreateTextureFromSurface(renderer, food_img_surface);
 	
	struct Snake snake;
	snake_init(&snake);
	
	struct Food food;
	random_food(&food, snake);

	SDL_Rect food_rect;
	food_rect.x = (food.x * 50);
	food_rect.y = (food.y * 50);
	food_rect.w = 50;
	food_rect.h = 50;


	unsigned int running = 1;
	while(running)
	{
		SDL_Delay(100);
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							if(snake.next_direction != DOWN)
								snake.next_direction = UP;
							break;

						case SDLK_DOWN:
							if(snake.next_direction != UP)
								snake.next_direction = DOWN;
							break;

						case SDLK_LEFT:
							if(snake.next_direction != RIGHT)
								snake.next_direction = LEFT;
							break;

						case SDLK_RIGHT:
							if(snake.next_direction != LEFT)
								snake.next_direction = RIGHT;
							break;

						case SDLK_ESCAPE:
							free(snake.tail);
							running = 0;
							break;
					
					}
					break;
				case SDL_QUIT:
					free(snake.tail);
					running = 0;
					break;
			
			}
		}

		game_update(&snake, &food, &food_rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		render_grid(renderer);
	

		snake_render(renderer, snake, head_img_texture);

		SDL_RenderCopy(renderer, food_img_texture, NULL, &food_rect);


		SDL_RenderPresent(renderer);

	}

	TTF_Quit();
	IMG_Quit();
	Mix_FreeMusic(ingame_music);
	Mix_CloseAudio();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

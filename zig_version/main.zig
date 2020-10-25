const std = @import("std");
const c = @cImport({
    @cDefine("_NO_CRT_STDIO_INLINE", "1");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("time.h");
    @cInclude("/usr/include/SDL2/SDL.h");
    @cInclude("/usr/include/SDL2/SDL_ttf.h");
    @cInclude("/usr/include/SDL2/SDL_image.h");
});

const allocator = std.heap.page_allocator;


const warn = std.debug.warn;

const SCREEN_WIDTH = 1000;
const SCREEN_HEIGHT = 1000;

const WHITE = c.SDL_Color {.r = 255, .g = 255, .b = 255, .a = 255,};
const RED   = c.SDL_Color {.r = 255, .g = 0, .b = 0, .a = 255,};
const GREEN = c.SDL_Color {.r = 0, .g = 255, .b = 0, .a = 255,};

const Point = struct {x: c_int, y: c_int};

const Direction = enum {UP,DOWN,LEFT,RIGHT};

const Snake = struct {position: Point, next_direction: Direction, total: u16, tail: []Point};

pub fn snake_init(snake_ptr: *Snake) anyerror!void
{
    snake_ptr.*.position.x = 10;
    snake_ptr.*.position.y = 10;
    snake_ptr.*.next_direction = Direction.RIGHT;
    snake_ptr.*.total = 0;

    snake_ptr.*.tail = try allocator.alloc(Point, 0);

    std.testing.expect(snake_ptr.*.tail.len == 0);
}

pub fn food_is_in_snake(food: Point, snake: Snake) bool
{

        for(snake.tail) |i|
        {
            if(i.x == food.x and i.y == food.y)
            {
                return true;
            } 
        }

        return false;
}


pub fn random_food(food_ptr: *Point, snake: Snake) void
{
    var last_food = food_ptr.*;

    food_ptr.*.x = @rem(c.rand(), @as(c_int, 20));
    food_ptr.*.y = @rem(c.rand(), @as(c_int, 20));

    while((food_ptr.*.x == last_food.x and food_ptr.*.y == last_food.y) or food_is_in_snake(food_ptr.*, snake) )
    {
            food_ptr.*.x = @rem(c.rand(), @as(c_int, 20));
            food_ptr.*.y = @rem(c.rand(), @as(c_int, 20));
    }

}


pub fn render_grid(renderer: ?*c.SDL_Renderer) void
{
    _ = c.SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

    var i: c_int = 1;
    while (i <= 20) : (i += 1)
    {
        _ = c.SDL_RenderDrawLine(renderer, i*50, 0, i*50, 1000);
        _ = c.SDL_RenderDrawLine(renderer, 0, i*50, 1000, i*50);
    } 

}

pub fn game_update(snake: *Snake, food: *Point, food_rect: *c.SDL_Rect) anyerror!void
{
    var last_snake = snake.*;

    switch(snake.*.next_direction)
    {
        Direction.UP =>
        {
            snake.*.position.y -= 1;
        },
        Direction.DOWN =>
        {
            snake.*.position.y += 1;
        },
        Direction.LEFT =>
        {
            snake.*.position.x -= 1;
        },
        Direction.RIGHT =>
        {
            snake.*.position.x += 1;
        },
    }

    if(19 < snake.*.position.x or 19 < snake.*.position.y or snake.*.position.x < 0 or snake.*.position.y < 0)
    {    
        allocator.free(snake.tail);
        try snake_init(snake);
    }

    var j: usize = 0;
    while(j < snake.*.total) : (j += 1)
    {
        if(snake.*.position.x == snake.*.tail[j].x and snake.*.position.y == snake.*.tail[j].y)
        {
            allocator.free(snake.tail);
            try snake_init(snake);
        }
    }

    if((snake.*.position.x == food.*.x) and (snake.*.position.y == food.*.y))
    {
        random_food(food, snake.*);

        food_rect.*.x = food.*.x * 50;
        food_rect.*.y = food.*.y * 50;

        snake.*.total += 1;
        
        warn("point = {}\n", .{snake.*.total});

        var new_tail = Point{.x = snake.*.position.x, .y = snake.*.position.y};
        snake.*.tail = try allocator.realloc(snake.*.tail, snake.*.total);
        snake.*.tail[snake.*.total - 1] = new_tail;
    }

    j = 0;
    while(j < snake.*.total) : (j += 1)
    {
        if(j > 0)
        {
            snake.*.tail[j-1].x = snake.*.tail[j].x;
            snake.*.tail[j-1].y = snake.*.tail[j].y;
        }
    }

    if(snake.*.total > 0)
    {
        snake.*.tail[snake.*.total-1].x = last_snake.position.x;
        snake.*.tail[snake.*.total-1].y = last_snake.position.y;
    }
    
}

pub fn snake_render(renderer: ?*c.SDL_Renderer, snake: Snake, head_img_texture: ?*c.SDL_Texture, snake_rect: *c.SDL_Rect) void
{
    _ = c.SDL_SetRenderDrawColor(renderer, 51, 255, 51, 0);

    snake_rect.*.x = snake.position.x * 50;
    snake_rect.*.y = snake.position.y * 50;

    _ = c.SDL_RenderFillRect(renderer, snake_rect);

    _ = c.SDL_RenderCopy(renderer, head_img_texture, null, snake_rect);

    var w: usize = comptime 0;
    while(w < snake.total) : (w += 1)
    {
        snake_rect.x = (snake.tail[w].x * 50);
        snake_rect.y = (snake.tail[w].y * 50);
        _ = c.SDL_RenderFillRect(renderer, snake_rect);
    }


}

pub fn main() anyerror!u8 {
    if (c.SDL_Init(c.SDL_INIT_VIDEO) != 0)
    {
        warn("error initializing SDL: {}\n", .{c.SDL_GetError()});
        return 1;
    }

    if(c.TTF_Init() != 0)
    {
        warn("error initializing TTF: {}\n", .{c.TTF_GetError()});
        return 1;
    }

    if(c.IMG_Init(c.IMG_INIT_PNG) & c.IMG_INIT_PNG != c.IMG_INIT_PNG)
    {
        warn("error initializing IMG: {}\n", .{c.IMG_GetError()});
        return 1;
    }

    var window = c.SDL_CreateWindow("Bald Snake", c.SDL_WINDOWPOS_CENTERED, c.SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == null)
    {
        warn("error initializing SDL: {}\n", .{c.SDL_GetError()});
        return 1;
    }

    var renderer = c.SDL_CreateRenderer(window, -1, c.SDL_RENDERER_ACCELERATED | c.SDL_RENDERER_PRESENTVSYNC);
    if(renderer == null)
    {
        warn("error creating renderer: {}\n", .{c.SDL_GetError()});
        return 1;
    }


    var event: c.SDL_Event = undefined;
  
    var isRunning: bool = true;

    var isInMenu: bool = true;

    var placeInMenu: u8 = 0; 
  

    var font = c.TTF_OpenFont("assets/fonts/Roboto-Black.ttf", 1000); 
    if(font == null)
    {
        warn("error opening font: {}\n", .{c.TTF_GetError()});
        return 1;
    }

    var surface_playtext = c.TTF_RenderText_Solid(font, "Play", RED);
    var texture_playtext = c.SDL_CreateTextureFromSurface(renderer, surface_playtext);

    var surface_quittext = c.TTF_RenderText_Solid(font, "Quit", WHITE);
    var texture_quittext = c.SDL_CreateTextureFromSurface(renderer, surface_quittext);

    
    
    var surface_banner = c.IMG_Load("assets/img/game_banner.png");
    if(surface_banner == null)
    {
        warn("cannot load img: {}\n", .{c.IMG_GetError()});
    }
    var texture_banner = c.SDL_CreateTextureFromSurface(renderer, surface_banner);

    var surface_footertext = c.TTF_RenderText_Solid(font, "don't judge pls", GREEN);
    var texture_footertext = c.SDL_CreateTextureFromSurface(renderer, surface_footertext);

    var play_rect = c.SDL_Rect{.x = 400, .y = 325, .w = 200, .h = 200};
    var quit_rect = c.SDL_Rect{.x = 400, .y = 500, .w = 200, .h = 200};
    var banner_image_rect = c.SDL_Rect{.x = 250, .y = 20, .w = 500, .h = 300};
    var footertext_rect = c.SDL_Rect{.x = 120, .y = 625, .w = 750, .h = 380};

    while(isInMenu)
    {
        _ = c.SDL_Delay(50);
        while(c.SDL_PollEvent(&event) != 0)
        {
            switch(event.type)
            {
                c.SDL_KEYDOWN =>
                {
                    switch(event.key.keysym.sym)
                    {
                       c.SDLK_UP =>
                       {
                            if(placeInMenu == 0) {break;}
                            placeInMenu -= 1;
                            surface_playtext = c.TTF_RenderText_Solid(font, "Play", RED);
                            texture_playtext = c.SDL_CreateTextureFromSurface(renderer, surface_playtext);
                            surface_quittext = c.TTF_RenderText_Solid(font, "Quit", WHITE);
                            texture_quittext = c.SDL_CreateTextureFromSurface(renderer, surface_quittext);
                            break;
                       },
                       c.SDLK_DOWN =>
                       {
                            if(placeInMenu == 1) break;
                            placeInMenu += 1;

                            surface_playtext = c.TTF_RenderText_Solid(font, "Play", WHITE);
                            texture_playtext = c.SDL_CreateTextureFromSurface(renderer, surface_playtext);
                            surface_quittext = c.TTF_RenderText_Solid(font, "Quit", RED);
                            texture_quittext = c.SDL_CreateTextureFromSurface(renderer, surface_quittext);
                            break;
                       },
                       c.SDLK_RETURN, c.SDLK_KP_ENTER, =>
                       {
                            if(placeInMenu == 0)
                            {isInMenu = false; c.SDL_FreeSurface(surface_playtext); c.SDL_FreeSurface(surface_quittext); c.SDL_FreeSurface(surface_banner); c.SDL_FreeSurface(surface_footertext);
                            c.SDL_DestroyTexture(texture_footertext);c.SDL_DestroyTexture(texture_banner); c.SDL_DestroyTexture(texture_playtext); c.SDL_DestroyTexture(texture_quittext); break;}
                            else
                            {
                                isInMenu = false; isRunning = false;    
                            }
                       },
                       c.SDLK_ESCAPE =>
                       {
                        isInMenu = false; isRunning = false;
                       },
                       else => {},
                    }
                },

                c.SDL_QUIT =>
                {
                    isInMenu = false;
                    isRunning = false;
                },
                else => {},
            }
        }

        _ = c.SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        _ = c.SDL_RenderClear(renderer);

        _ = c.SDL_RenderCopy(renderer, texture_banner, null, &banner_image_rect);
        _ = c.SDL_RenderCopy(renderer, texture_playtext, null, &play_rect);
        _ = c.SDL_RenderCopy(renderer, texture_quittext, null, &quit_rect);
        _ = c.SDL_RenderCopy(renderer, texture_footertext, null, &footertext_rect);
        _ = c.SDL_RenderPresent(renderer);
    }


    var head_img_surface = c.IMG_Load("assets/img/head.png");
    if(head_img_surface == null)
    {
        warn("cannot load head.png: {}", .{c.IMG_GetError()});
        return 1;
    }

    const head_img_texture: ?*c.SDL_Texture = c.SDL_CreateTextureFromSurface(renderer, head_img_surface);

    var food_img_surface = c.IMG_Load("assets/img/food.png");
    if(food_img_surface == null)
    {
        warn("cannot load food.png: {}", .{c.IMG_GetError()});
        return 1;
    }
    var food_img_texture = c.SDL_CreateTextureFromSurface(renderer, food_img_surface);


    var snake_rect = c.SDL_Rect{.x = undefined, .y = undefined, .w = 50, .h = 50}; 

    var snake = Snake{.position = Point{.x = 0, .y = 0}, .next_direction = Direction.RIGHT, .total = 0, .tail = undefined};
    try snake_init(&snake);

    c.srand(@intCast(u32, c.time(0)));

    var food: Point = undefined;
    random_food(&food, snake);

    var food_rect = c.SDL_Rect{.x = food.x * 50, .y = food.y * 50, .w = 50, .h = 50};

    while(isRunning)
    {
        _ = c.SDL_Delay(100);

        while(c.SDL_PollEvent(&event) != 0)
        {
            switch(event.type)
            {
                c.SDL_KEYDOWN =>
                {
                    switch(event.key.keysym.sym)
                    {
                        c.SDLK_UP =>
                        {
                            if(snake.next_direction != Direction.DOWN) snake.next_direction = Direction.UP else break;
                        },
                        c.SDLK_DOWN =>
                        {
                            if(snake.next_direction != Direction.UP) snake.next_direction = Direction.DOWN else break;
                        },
                        c.SDLK_LEFT =>
                        {
                            if(snake.next_direction != Direction.RIGHT) snake.next_direction = Direction.LEFT else break;                     
                        },
                        c.SDLK_RIGHT =>
                        {
                            if(snake.next_direction != Direction.LEFT) snake.next_direction = Direction.RIGHT else break;
                        },
                        c.SDLK_ESCAPE =>
                        {
                            allocator.free(snake.tail);
                            isRunning = false;
                        },
                        else => {}, 
                    }                   
                },
                c.SDL_QUIT =>
                {
                    allocator.free(snake.tail);
                    isRunning = false; 
                },
                else => {}
            }
        
        }

        try game_update(&snake, &food, &food_rect);

        _ = c.SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        _ = c.SDL_RenderClear(renderer);

        render_grid(renderer);

        snake_render(renderer, snake, head_img_texture, &snake_rect);

        _ = c.SDL_RenderCopy(renderer, food_img_texture, null, &food_rect);

        _ = c.SDL_RenderPresent(renderer);



    } 

    _ = c.TTF_Quit();

    _ = c.IMG_Quit();

    _ = c.SDL_DestroyRenderer(renderer);
    _ = c.SDL_DestroyWindow(window);
    _ = c.SDL_Quit();

    return 0;
}

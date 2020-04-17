#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include "../include/Global.h"
#include "../include/concat.h"
#include "../include/content.h"

enum {WINDOW_DEFAULT_WIDTH = 1280};
enum {WINDOW_DEFAULT_HEIGHT = 720};
enum {NO_FLAGS = 0};
enum {GET_BEST_RENDERER_AVAILABLE = -1};
enum {TITLE_SCREEN = 0};

int VALID_KEYS[8] = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D};

void initialize(Global *global) {
    if (!global->sprites)
        SDL_Log("Allocation error: the game was unable to allocate %d bytes in the heap", sizeof(SDL_Texture*) * NUM_OF_CONTENTS);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        SDL_Log("SDL initialization error: %s", SDL_GetError());
        exit(-1);
    }
    atexit(SDL_Quit);

    if ( !( global->Window.window = SDL_CreateWindow("Ao Topo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT,
    SDL_WINDOW_RESIZABLE) ) ) {
        SDL_Log("SDL window creation error: %s", SDL_GetError());
        exit(-1);
    }

    if ( !( global->renderer = SDL_CreateRenderer(global->Window.window, GET_BEST_RENDERER_AVAILABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) ) ) {
        SDL_Log("SDL renderer creation error: %s", SDL_GetError());
        global->should_quit = SDL_TRUE;
        global->exit_code = EXIT_FAILURE;
    }

    SDL_SetRenderDrawColor(global->renderer, 0, 0, 255, 0);

    if ( !(global->base_path = SDL_GetBasePath()) ) {
        SDL_Log("SDL base path retrieving error: %s", SDL_GetError());
        global->should_quit = SDL_TRUE;
        global->exit_code = EXIT_FAILURE;
    }
}

void load_content(Global* global, char *contents[]) {
    char *buffer = (char*) malloc(strlen(global->base_path) + 255);
    SDL_Surface *surfaces[NUM_OF_CONTENTS];

    for (int i = 0; i < NUM_OF_CONTENTS; i++) {
        if ( !( surfaces[i] = SDL_LoadBMP(concat(buffer, global->base_path, contents[i])) ) ) {
            SDL_Log("Surface creation error: %s", SDL_GetError());
            global->should_quit = SDL_TRUE;
            global->exit_code = EXIT_FAILURE;
            break;
        }
        SDL_SetColorKey(surfaces[i], SDL_TRUE, SDL_MapRGB(surfaces[i]->format, 6, 6, 6));
        *(global->sprites + (i * sizeof(SDL_Texture*))) = SDL_CreateTextureFromSurface(global->renderer, surfaces[i]);
        SDL_SetTextureBlendMode(*(global->sprites + (i * sizeof(SDL_Texture*))), SDL_BLENDMODE_BLEND);
    }
    free(buffer);
}

void update(Global *global) {
    SDL_PollEvent(global->event);
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    global->tick_count = SDL_GetTicks();
    if (keyboard_state[SDL_SCANCODE_ESCAPE] || global->event->type == SDL_QUIT) {
        global->should_quit = SDL_TRUE;
        global->exit_code = EXIT_SUCCESS;
    }
    else if (global->event->type == SDL_WINDOWEVENT && global->event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        SDL_GetWindowSize(global->Window.window, &(global->Window.width), &(global->Window.height));
    }
    else if (keyboard_state[SDL_SCANCODE_F]) SDL_SetWindowFullscreen(global->Window.window, SDL_WINDOW_FULLSCREEN);

    static int acceleration = 10;
    static int speed = 0;
    // static SDL_bool still = SDL_TRUE;
    //if (acceleration != 10) acceleration++;
    if (global->stage == TITLE_SCREEN)
        for (int i = 0; i < 8; i++)
            if (keyboard_state[VALID_KEYS[i]])
                global->stage = 1;
    if (global->stage == 1) {
        if (global->rects[DINO_00].y > global->Window.height - 2 * SIZES[GROUND_TILE][HEIGHT] && global->speeds[DINO_00] != 0) {
            global->speeds[DINO_00] = 0;
            global->rects[DINO_00].y--;
        }
        if (global->rects[DINO_00].x > global->Window.height - 2 * SIZES[GROUND_TILE][HEIGHT] && speed != 0) {
            speed = 0;
            global->rects[DINO_00].x--;
        }
        if (global->rects[SKY].y == global->Window.height) global->rects[SKY].y = 0;

        if (keyboard_state[SDL_SCANCODE_UP]) {
            //global->rects[BACKGROUND].y += acceleration; // = global->entities[BACKGROUND].y++;
            global->rects[SKY].y += acceleration;
            global->speeds[DINO_00] += acceleration / 10;
        }
        else {
//            if (global->rects[DINO_00].y < )
                global->rects[DINO_00].y -= global->speeds[DINO_00];
            global->speeds[DINO_00]--;
        }
        if (keyboard_state[SDL_SCANCODE_LEFT]) {
            global->rects[SKY].x += acceleration;
            speed += acceleration / 10;
        }
        else {
//            if (global->rects[DINO_00].y < )
                global->rects[DINO_00].x -= speed;
            speed--;
        }
    }
}

#define GET_TEXTURE(NAME) *(global->sprites + (NAME * sizeof(SDL_Texture*)))
#define DRAW(NAME, X, Y) buffer_rect.x = X; buffer_rect.y = Y; buffer_rect.w = (int) ( ((float) SIZES[NAME][WIDTH]) / ((float) WINDOW_DEFAULT_WIDTH) * global->Window.width ); buffer_rect.h = (int) ( (float) SIZES[NAME][HEIGHT] / (float) WINDOW_DEFAULT_HEIGHT * global->Window.height); SDL_RenderCopy(global->renderer, GET_TEXTURE(NAME), NULL, &buffer_rect)
void draw(Global *global) {
    SDL_Rect buffer_rect;
    SDL_Texture *buffer_texture;
    SDL_RenderClear(global->renderer);

    if (global->stage == TITLE_SCREEN) {
        SDL_RenderCopy(global->renderer, GET_TEXTURE(ICE_MOUNTAIN), NULL, NULL);
        DRAW(TITLE, (global->Window.width - SIZES[TITLE][WIDTH]) / 2, (global->Window.height / 4));
        DRAW(PRESS_ARROW_OR_WASD, (global->Window.width - SIZES[PRESS_ARROW_OR_WASD][WIDTH]) / 2, (global->Window.height / 4) * 3);
    }
    else if (global->stage == 1) {
        DRAW(SKY, global->rects[SKY].x - global->Window.width, global->rects[SKY].y - global->Window.height);
        DRAW(SKY, global->rects[SKY].x, global->rects[SKY].y);
        DRAW(SKY, global->rects[SKY].x + global->Window.width, global->rects[SKY].y - global->Window.height);

        DRAW(SKY, global->rects[SKY].x, global->rects[SKY].y - global->Window.height);
        DRAW(SKY, global->rects[SKY].x, global->rects[SKY].y);
        DRAW(SKY, global->rects[SKY].x, global->rects[SKY].y + global->Window.height);

//        DRAW(BACKGROUND, global->rects[BACKGROUND].x, global->rects[BACKGROUND].y);
        for (int x = 0; x < global->Window.width; x += global->Window.width)
            DRAW(GROUND_TILE, x, global->Window.height - SIZES[GROUND_TILE][HEIGHT]);
        DRAW(DINO_00, global->rects[DINO_00].x, global->rects[DINO_00].y);
    }

    SDL_RenderPresent(global->renderer);
}

void clean_up(Global *global) {
    SDL_DestroyWindow(global->Window.window);
    if (global->renderer) SDL_DestroyRenderer(global->renderer);
    if (global->base_path) free(global->base_path);
    if (global->sprites) free(global->sprites);
}

int main(void) {
    Global global = {
        .Window = {
            .window = NULL,
            .width = WINDOW_DEFAULT_WIDTH,
            .height = WINDOW_DEFAULT_HEIGHT,
        },
        .renderer = NULL,
        .base_path = NULL,
        .should_quit = SDL_FALSE,
        .exit_code = -1,
        .sprites = (SDL_Texture**) malloc(sizeof(SDL_Texture*) * NUM_OF_CONTENTS),
        .event = NULL,
        .tick_count = 0,
        .stage = TITLE_SCREEN,
        .rects = {0},
        .speeds = {0}
    };

    initialize(&global);

    // CONTENT_PATHS is defined in content.h
    load_content(&global, CONTENT_PATHS);

    global.rects[SKY].y = -1 * global.Window.height;
    global.rects[DINO_00].y = global.Window.height - (2 * SIZES[GROUND_TILE][HEIGHT]);
    SDL_Event event;
    global.event = &event;
    while (!global.should_quit) {
        update(&global);
        if (global.should_quit)
            break;
        draw(&global);
    }

    return global.exit_code;
}

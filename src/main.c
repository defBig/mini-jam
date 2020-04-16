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

void initialize(Global *global) {
    if (!global->sprites) {
        SDL_Log("Allocation error: the game was unable to allocate %d bytes in the heap", sizeof(SDL_Texture*) * NUM_OF_CONTENTS);
    }

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
        surfaces[i] = SDL_LoadBMP(concat(buffer, global->base_path, contents[i]));
        if (!surfaces[i]) {
            SDL_Log("Surface creation error: %s", SDL_GetError());
            global->should_quit = SDL_TRUE;
            global->exit_code = EXIT_FAILURE;
            break;
        }
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
}

#define GET_TEXTURE(NAME) *(global->sprites + (NAME * sizeof(SDL_Texture*)))
#define DRAW(NAME, X, Y) buffer_rect.x = X; buffer_rect.y = Y; buffer_rect.w = (int) ( ((float) SIZES[NAME][WIDTH]) / ((float) WINDOW_DEFAULT_WIDTH) * global->Window.width ); buffer_rect.h = (int) ( (float) SIZES[NAME][HEIGHT] / (float) WINDOW_DEFAULT_HEIGHT * global->Window.height); SDL_RenderCopy(global->renderer, GET_TEXTURE(NAME), NULL, &buffer_rect)
void draw(Global *global) {
    SDL_Rect buffer_rect;
    SDL_Texture *buffer_texture;
    SDL_RenderClear(global->renderer);
    SDL_RenderCopy(global->renderer, GET_TEXTURE(ICE_MOUNTAIN), NULL, NULL);
    /*
    // SIZES is declared in content.h
    if (global->tick_count % 255 < 50) SDL_SetTextureAlphaMod(GET_TEXTURE(TITLE), global->tick_count % 255 + 50);
    else SDL_SetTextureAlphaMod(GET_TEXTURE(TITLE), global->tick_count % 255);
    */
    DRAW(TITLE, (global->Window.width - SIZES[TITLE][WIDTH]) / 2, (global->Window.height / 4));
    //unsigned int tick_snapshot = global->tick_count / 10;
/*
    if (tick_snapshot % 255 == 50) {
        if (first_run) first_run = SDL_FALSE;
        else {
            descending = !descending;
            SDL_SetTextureAlphaMod(GET_TEXTURE(PRESS_ANY_KEY), 50);
        }
    }
    else if (tick_snapshot % 255 == 0) {
        descending = !descending;
        SDL_SetTextureAlphaMod(GET_TEXTURE(PRESS_ANY_KEY), 255);
    }
    else if (tick_snapshot % 255 < 50 && descending) {
        SDL_Log("%d", tick_snapshot % 255);
        SDL_SetTextureAlphaMod(GET_TEXTURE(PRESS_ANY_KEY), tick_snapshot % 255);
    }
    else
        SDL_SetTextureAlphaMod(GET_TEXTURE(PRESS_ANY_KEY), tick_snapshot % 255);
*/
    static int i = 0;
    if (global->tick_count / 1000 / 255 & 1) i = 255 - global->tick_count / 1000 % 255;
    else i = global->tick_count / 1000 % 255;
    SDL_SetTextureAlphaMod(GET_TEXTURE(PRESS_ANY_KEY), i);
    DRAW(PRESS_ANY_KEY, (global->Window.width - SIZES[PRESS_ANY_KEY][WIDTH]) / 2, global->Window.height / 4 * 3);
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
        .tick_count = 0
    };

    initialize(&global);

    // CONTENT_PATHS is defined in content.h
    load_content(&global, CONTENT_PATHS);

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

#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include "Global.h"
#include "concat.h"
#include "content.h"

enum {WINDOW_DEFAULT_WIDTH = 1280};
enum {WINDOW_DEFAULT_HEIGHT = 720};
enum {NO_FLAGS = 0};
enum {GET_BEST_RENDERER_AVAILABLE = -1};
enum {ICE_MOUNTAIN_SPRITE};

void initialize(Global *global) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
        SDL_Log("SDL initialization error: %s", SDL_GetError());
    atexit(SDL_Quit);

    if ( !( global->Window.window = SDL_CreateWindow("Ao Topo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE) ) )
        SDL_Log("SDL window creation error: %s", SDL_GetError());

    if ( !( global->renderer = SDL_CreateRenderer(global->Window.window, GET_BEST_RENDERER_AVAILABLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) ) )
        SDL_Log("SDL renderer creation error: %s", SDL_GetError());

    global->base_path = SDL_GetBasePath();
}

void load_content(Global* global, char *contents[], SDL_Texture *sprites[], char *buffer) {
    SDL_Surface *surfaces[NUM_OF_CONTENTS];
    for (int i = 0; i < NUM_OF_CONTENTS; i++) {
        surfaces[i] = SDL_LoadBMP(concat(buffer, global->base_path, contents[i]));
        if (surfaces[i] == 0) {
            global->should_quit = true;
            global->exit_code = EXIT_FAILURE;
            SDL_Log("%s", SDL_GetError());
            break;
        }
        // Second parameter enables color key.
        // SDL_SetColorKey(surfaces[i], SDL_TRUE, SDL_MapRGB((surfaces[i])->format, 6, 6, 6));
        sprites[i] = SDL_CreateTextureFromSurface(global->renderer, surfaces[i]);
    }
}

void update(Global *global, SDL_Event *event) {
    SDL_PollEvent(event);
    SDL_GetWindowSize(global->Window.window, &(global->Window.width), &(global->Window.height));
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_ESCAPE] || event->type == SDL_QUIT) {
        global->should_quit = true;
        global->exit_code = EXIT_SUCCESS;
    }
}

#define DRAW(NAME, X, Y) buffer_rect.x = X; buffer_rect.y = Y; buffer_rect.w = SIZES[NAME][WIDTH]; buffer_rect.h = SIZES[NAME][HEIGHT]; SDL_RenderCopy(global->renderer, sprites[NAME], NULL, &buffer_rect)
void draw(Global *global, SDL_Texture *sprites[]) {
    SDL_Rect buffer_rect;
    SDL_Texture *buffer_texture;
    SDL_RenderClear(global->renderer);
    SDL_RenderCopy(global->renderer, sprites[ICE_MOUNTAIN_SPRITE], NULL, NULL);
    // SIZES is declared in content.h
    DRAW(TITLE, (global->Window.width - SIZES[TITLE][WIDTH]) / 2, (global->Window.height / 4));
    //_rect(&buffer_rect, (WINDOW_DEFAULT_WIDTH - SIZES[TITLE][WIDTH]) / 2, (WINDOW_DEFAULT_HEIGHT / 16), SIZES[TITLE][WIDTH], SIZES[TITLE][HEIGHT])
    //SDL_RenderCopy(&buffer_rect)
    DRAW(PRESS_ANY_KEY, (global->Window.width - SIZES[PRESS_ANY_KEY][WIDTH]) / 2, global->Window.height / 4 * 3);
    SDL_RenderPresent(global->renderer);
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
        .should_quit = false,
        .exit_code = 0,
    };

    initialize(&global);
    // CONTENT_PATHS and NUM_OF_CONTENTS are defined in content.h

    char *contents[] = CONTENT_PATHS;
    SDL_Texture *sprites[NUM_OF_CONTENTS];
    void *buffer = malloc(strlen(global.base_path) + 255);
    load_content(&global, contents, sprites, (char*) buffer);

    SDL_Event event;
    global.exit_code = -1;
    SDL_SetRenderDrawColor(global.renderer, 0, 0, 255, 0);

    while (!global.should_quit) {
        update(&global, &event);
        if (global.should_quit)
            break;
        draw(&global, sprites);
    }

    SDL_DestroyWindow(global.Window.window);
    SDL_DestroyRenderer(global.renderer);
    free(global.base_path);
    free(buffer);
    SDL_Quit();
    return global.exit_code;
}

#ifndef GLOBAL_H
#define GLOBAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "content.h"

typedef struct Global {
    struct Window {
        SDL_Window *window;
        int width;
        int height;
        SDL_bool fullscreen;
    } Window;
    SDL_Renderer *renderer;
    char *base_path;
    SDL_bool should_quit;
    int exit_code;
    SDL_Event *event;
    SDL_Texture **sprites;
} Global;

#endif

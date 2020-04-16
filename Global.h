#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct Global {
    struct Window {
        SDL_Window *window;
        int width;
        int height;
    } Window;
    SDL_Renderer *renderer;
    char *base_path;
    bool should_quit;
    int exit_code;
} Global;

#endif

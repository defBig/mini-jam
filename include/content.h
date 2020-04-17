#ifndef CONTENT_H
#define CONTENT_H

#ifdef __WIN32
char *CONTENT_PATHS[] = {"Content\ice_mountain.bmp", "Content\title.bmp", "Content\press_arrow_or_wasd.bmp", "Content\background.bmp", "Content\dino\parts00.bmp", "Content\ground_tile.bmp", "Content\sky.bmp"};
#else
char *CONTENT_PATHS[] = {"Content/ice_mountain.bmp", "Content/title.bmp", "Content/press_arrow_or_wasd.bmp", "Content/background.bmp", "Content/dino/parts00.bmp", "Content/ground_tile.bmp", "Content/sky.bmp"};
#endif

#define NUM_OF_CONTENTS 7

enum {ICE_MOUNTAIN, TITLE, PRESS_ARROW_OR_WASD, BACKGROUND, DINO_00, GROUND_TILE, SKY};
enum {WIDTH, HEIGHT};
int const SIZES[NUM_OF_CONTENTS][2] = {
    {1280, 720}, {943, 90}, {1072, 40}, {1800, 893}, {160, 171}, {128, 128}, {1800, 893}
};

#endif

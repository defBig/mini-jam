#ifndef CONTENT_H
#define CONTENT_H

#ifdef __WIN32
char *CONTENT_PATHS[] = {"Content\ice_mountain.bmp","Content\title.bmp","Content\press_any_key.bmp"};
#else
char *CONTENT_PATHS[] = {"Content/ice_mountain.bmp","Content/title.bmp","Content/press_any_key.bmp"};
#endif

#define NUM_OF_CONTENTS 3

enum {ICE_MOUNTAIN, TITLE, PRESS_ANY_KEY};
enum {WIDTH, HEIGHT};
int const SIZES[NUM_OF_CONTENTS][2] = {
    {1280, 720}, {523, 90}, {394, 36}
};

#endif

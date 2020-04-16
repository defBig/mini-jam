#include <string.h>

char* concat(char *buffer, char *str1, char *str2) {
    strcpy(buffer, str1);
    strcat(buffer, str2);
    return buffer;
}

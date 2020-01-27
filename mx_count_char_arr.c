#include "ush.h"

int count_char_array(char **arr) {
    int i = 0;

    if (arr == NULL)
        return (0);
    while (arr[i] != NULL)
        i++;
    return i;
}

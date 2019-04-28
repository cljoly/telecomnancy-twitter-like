#include <stdio.h>
#include <string.h>
#include "tools.h"

size_t read_stdin(char* input, int max_size){
    fgets(input, max_size, stdin);

    size_t nLen = strlen(input);

    if (input[nLen - 1] == '\n') {
        input[nLen - 1] = '\0';
    } else {
        if(!feof(stdin)) {
            int c = 0;
            while ((c = fgetc(stdin)) != '\n' && (c != EOF));
        }
    }
    return nLen;
}


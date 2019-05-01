#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tools.h"

size_t read_stdin(char* input, int max_size){
    fgets(input, max_size, stdin);

    const size_t nLen = strlen(input);

    if (input[nLen - 1] == '\n') {
        input[nLen - 1] = '\0';
    } else {
        if(!feof(stdin)) {
            int c = 0;
            while ((c = fgetc(stdin)) != '\n' && (c != EOF));
        }
    }
    return strlen(input);
}

void get_iso_time_now(char* result){
    time_t timer;
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(result, 26, "%Y-%m-%dT%H:%M:%S", tm_info);
}

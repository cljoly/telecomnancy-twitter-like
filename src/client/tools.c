#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tools.h"

size_t read_stdin(char* input, int max_size){
    if(feof(stdin)) {
        exit(0);
    }
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


unsigned int string_to_unsigned_int(char* string) {
    char* endptr;
    unsigned int input = (unsigned int) strtoul(string, &endptr, 10);
    if (endptr == string) {
        return UINT_MAX;
    } else {
        return input;
    }
}
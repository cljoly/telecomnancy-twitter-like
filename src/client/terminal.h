#ifndef PROJET_RSA_DEDONATO_JOLY_TERMINAL_H
#define PROJET_RSA_DEDONATO_JOLY_TERMINAL_H

#include <stdio.h>

typedef enum message_type_t {SUCCESS, INFO, ERROR, FATAL_ERROR, DEBUG} message_type_t;
// Fonctions d'affichage
void clear_above_below_positions();
void clear_all_terminal();
void clear_terminal_except_header();
void clear_above_messages();
void print_menu(unsigned int first_command_index, unsigned int last_command_index);
void print_message(message_type_t type, const char* format, va_list args);
void print_message_above(message_type_t type, const char* format, ...);
void print_message_below(message_type_t type, const char* format, ...);
void clean_line(unsigned int line);
void print_cleaned_line(unsigned int terminal_row, const char* format, ...);
unsigned int prompt_user(int connected);
int prompt_user_for_parameter(const char* prompt, char* result, size_t bufsize,  unsigned int parameter_index);

void print_title();

#endif //PROJET_RSA_DEDONATO_JOLY_TERMINAL_H

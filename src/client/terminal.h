//
// Created by laury on 28/04/19.
//

#ifndef PROJET_RSA_DEDONATO_JOLY_TERMINAL_H
#define PROJET_RSA_DEDONATO_JOLY_TERMINAL_H

typedef enum message_type_t {SUCCESS, INFO, ERROR, FATAL_ERROR, DEBUG} message_type_t;
// Fonctions d'affichage
void clear_all_terminal();
void clear_terminal_exceptHeader();
void print_menu(int connected);
void print_message(message_type_t type, const char* format, ...);
unsigned int prompt_user(int connected);
int prompt_user_for_parameter(const char* prompt, char* result);

void printTitle();

#endif //PROJET_RSA_DEDONATO_JOLY_TERMINAL_H

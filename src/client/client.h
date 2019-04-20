//
// Created by laury on 18/04/19.
//

#ifndef PROJET_RSA_DEDONATO_JOLY_CLIENT_H
#define PROJET_RSA_DEDONATO_JOLY_CLIENT_H

#define DEFAULT_PORT 1234
#define MAXDATASIZE 140

static char* commands[] = {
        "hello              ",
        "create_account     ",
        "connect            ",
        "send_gazou         ",
        "relay_gazou        ",
        "follow_user        ",
        "follow_tag         ",
        "unfollow_user      ",
        "unfollow_tag       ",
        "list_followed_users",
        "list_followed_tags ",
        "list_my_followers  ",
        "disconnect         ",
        "update             ",
};

/**
 * Une request_function est une fonction qui peut être appellée à la demande de l'utilisateur.
 * Elle doit retourner 0 en cas de succès, 1 en cas d'erreur. Elle est responsable d'afficher un message d'erreur.
 * TODO: gérer les paramètres des requêtes
 */
typedef int(* request_function)(void*);

int not_implemented();

int test_hello(void* sockfd_ptr);

int disconnect(void* sockfd_ptr);

static request_function functions[] = {
        test_hello,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        not_implemented,
        disconnect,
        not_implemented
};
static const unsigned int functions_count = sizeof(functions) / sizeof(request_function);
static const unsigned int commands_count = sizeof(commands) / sizeof(char*);


// Fonctions d'affichage
void clearTerminal();
void print_menu();
unsigned int prompt_user();

// Fonctions de communication réseau
int init_connection(struct hostent* server);

void usage();

#endif //PROJET_RSA_DEDONATO_JOLY_CLIENT_H

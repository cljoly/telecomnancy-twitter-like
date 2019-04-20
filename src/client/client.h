//
// Created by laury on 18/04/19.
//

#ifndef PROJET_RSA_DEDONATO_JOLY_CLIENT_H
#define PROJET_RSA_DEDONATO_JOLY_CLIENT_H

#define DEFAULT_PORT 1234
#define MAXDATASIZE 140

// Forward-declaration de json_object pour ne pas mettre d'include dans le .h
typedef struct json_object json_object;


char* commands[] = {
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
 * Elle demande les éventuels paramètres nécessaires à l'utilisateur.
 * TODO: gérer les paramètres des requêtes
 */
typedef int(* request_function)(int sockfd_ptr);

int not_implemented();

int test_hello(int sockfd_ptr);
int create_account(int sockfd);
int disconnect(int sockfd_ptr);

request_function functions[] = {
        test_hello,
        create_account,
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
const unsigned int functions_count = sizeof(functions) / sizeof(request_function);
const unsigned int commands_count = sizeof(commands) / sizeof(char*);

json_object* create_request(const char* method);
int fill_request(json_object* request, const char** params_name);




// Fonctions d'affichage
void clearTerminal();
void print_menu();
unsigned int prompt_user();
int prompt_user_for_parameter(const char* prompt, char* result);


// Fonctions de communication réseau
int init_connection(const struct hostent* server);
int send_message(int sockfd, const char* message);
char* get_response(int sockfd, char* buf, size_t bufsize);

void usage();

#endif //PROJET_RSA_DEDONATO_JOLY_CLIENT_H

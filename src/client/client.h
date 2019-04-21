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
        "Créer un compte",
        "Se connecter",
        "Envoyer un gazouilli       ",
        "Relayer un gazouilli",
        "Suivre un utilisateur",
        "Ne plus suivre un utilisateur",
        "Utilisateurs suivis",
        "Mes Abonnés",
        "Suivre une thématique",
        "Ne plus suivre une thématique",
        "Thématique suivies ",
        "Déconnexion"
};

/**
 * Une request_function est une fonction qui peut être appellée à la demande de l'utilisateur.
 * Elle demande les éventuels paramètres nécessaires à l'utilisateur, puis
 * créé et envoie la requête, et enfin lit et gère la réponse.
 *
 * @return 0 en cas de succès,
 *         le code d'erreur reçu en cas d'erreur
 *         <0 en cas d'erreur dans le message
 *  Elle est responsable d'afficher un message d'erreur pour l'utilisateur
 */
typedef int(* request_function)(int sockfd_ptr);

int not_implemented();

int create_account(int sockfd);
int disconnect(int sockfd_ptr);

request_function functions[] = {
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
json_object* get_response_object(int sockfd);
int check_response(json_object* response, unsigned int request_id);
int get_response_result(int sockfd, unsigned int id, json_object** result);

void usage();

#endif //PROJET_RSA_DEDONATO_JOLY_CLIENT_H

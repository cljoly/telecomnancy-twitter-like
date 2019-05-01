#ifndef PROJET_RSA_DEDONATO_JOLY_CLIENT_H
#define PROJET_RSA_DEDONATO_JOLY_CLIENT_H

#include "spec_const.h"
#define MAXDATASIZE 140

// Forward-declaration de json_object pour ne pas mettre d'include dans le .h
typedef struct json_object json_object;

// Forward-declaration de struct addr_info pour ne pas mettre d'include dans le .h
struct addrinfo;




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
typedef int(* request_function)();

int not_implemented();
int quit();
int create_account();
int disconnect();


json_object* create_request(const char* method);
int fill_request(json_object* request, const char** params_name);

// Fonctions de communication réseau
struct hostent;
const struct addrinfo* init_connection(const struct addrinfo* server_info);
int send_message(const char* message);
json_object* get_response_object();
int check_response(json_object* response, unsigned int request_id);
int get_response_result(unsigned int id, json_object** result);

void usage();
void force_quit();

#endif //PROJET_RSA_DEDONATO_JOLY_CLIENT_H

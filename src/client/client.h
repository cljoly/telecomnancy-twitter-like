#ifndef PROJET_RSA_DEDONATO_JOLY_CLIENT_H
#define PROJET_RSA_DEDONATO_JOLY_CLIENT_H

#include "spec_const.h"
#define MAXDATASIZE 140

// Forward-declaration de json_object pour ne pas mettre d'include dans le .h
typedef struct json_object json_object;

// Forward-declaration de struct addr_info pour ne pas mettre d'include dans le .h
struct addrinfo;

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

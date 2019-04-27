#ifndef JSON_COMMUNICATION_H_8ZET1UXD
#define JSON_COMMUNICATION_H_8ZET1UXD

#include <json.h>

// Module contenant les fonctions utiles pour la communication client/serveur
// en JSON

json_object* create_answer(json_object* request, int error_code);

json_object *get_request_object(char *json_string);

#endif /* end of include guard: JSON_COMMUNICATION_H_8ZET1UXD */

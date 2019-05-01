#ifndef PROJET_RSA_DEDONATO_JOLY_METHODS_H
#define PROJET_RSA_DEDONATO_JOLY_METHODS_H


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
int connect_server();
int disconnect();
typedef struct json_object json_object;

json_object* create_request(const char* method);
int fill_request(json_object* request, const char** params_name);
request_function get_function(unsigned int user_input);

#endif //PROJET_RSA_DEDONATO_JOLY_METHODS_H

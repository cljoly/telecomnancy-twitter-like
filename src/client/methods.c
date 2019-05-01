#include "methods.h"
#include "terminal.h"
#include "client.h"
#include <json.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

extern int connected;
extern int sockfd;

request_function functions[] = {
        quit,
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
        disconnect
};
const unsigned int functions_count = sizeof(functions) / sizeof(request_function);


/**
 * Fonction de requête précisant que la commande voulue n'est pas encore implémentée
 * @return 1
 */
int not_implemented() {
    print_message(FATAL_ERROR, "Commande non implémentée");
    return 1;
}

int create_account() {
    // Création de la requête
    json_object* request = create_request("create_account");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "username",
            "password",
            NULL
    };
    printf("Création d'un compte\n\n");
    fill_request(request, params);
    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    switch (error_code) {
        case 1: //erreur de notre doc : elle est gérée, on met l'error_code à 0 et on continue
            print_message(ERROR, "Ce nom d'utilisateur existe déjà.\n");
            error_code = 0;
            break;
        case 0:
            print_message(SUCCESS, "Compte créé!\n");
            break;
        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

/**
 * Ferme la socket et quitte le programme.
 * TODO: déconnexion propre
 * @return
 */
int disconnect() {
    close(sockfd);
    exit(0);
}

/**
 * Ferme la socket et quitte le programme
 * @return
 */
int quit(){
    close(sockfd);
    exit(0);
}



/********************************************************/

/**
 * Fonction générique préparant un object JSON de requête avec un identifiant unique.
 * Cette fonction ne gère pas les paramètres
 * @param method le nom la méthode de requête
 * @return l'object JSON pré-rempli
 */
json_object* create_request(const char* method) {
    static unsigned int id = 0;
    json_object* request = json_object_new_object();
    json_object_object_add(request, "request", json_object_new_string(method));
    json_object_object_add(request, "id", json_object_new_int(id));
    id++;
    return request;
}

int fill_request(json_object* request, const char** params_name) {

    char buf[MAXDATASIZE];
    json_object* params = json_object_new_object();
    for (int i = 0; params_name[i] != NULL; i++) {
        memset(buf, 0, MAXDATASIZE);
        if (prompt_user_for_parameter(params_name[i], buf) != 0) {
            return 1;
        }
        if (json_object_object_add(params, params_name[i], json_object_new_string(buf)) != 0) {
            return 2;
        }
    }
    if (json_object_object_add(request, "params", params) != 0) {
        return 3;
    }
    return 0;
}


/**
 * Retourne la fonction correspondant au numéro donné
 * @param user_input le numéro de fonction voulu
 * @return la fonction demandée, ou not_implemented si elle n'existe pas.
 */
request_function get_function(unsigned int user_input) {
    if (user_input + connected > functions_count) {
        return not_implemented;
    } else {
        return functions[user_input];
    }
}
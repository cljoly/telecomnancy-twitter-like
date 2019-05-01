#include "methods.h"
#include "terminal.h"
#include "client.h"
#include "tools.h"
#include <json.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

extern int cookie;
extern int sockfd;

request_function functions[] = {
        quit,               //"Quitter",
        create_account,     //"Créer un compte",
        connect_server,     //"Se connecter",
        send_gazou,         //"Envoyer un gazouilli         ",
        not_implemented,    //"Relayer un gazouilli         ",
        follow_user,        //"Suivre un utilisateur        ",
        unfollow_user,      //"Ne plus suivre un utilisateur",
        not_implemented,    //"Utilisateurs suivis          ",
        not_implemented,    //"Mes Abonnés                  ",
        follow_tag,         //"Suivre une thématique        ",
        unfollow_tag,       //"Ne plus suivre une thématique",
        not_implemented,    //"Thématique suivies           ",
        disconnect          //"Déconnexion                  "
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
        case 0:
            print_message(SUCCESS, "Compte créé !\n");
            break;

        case 1: //erreur de notre doc : elle est gérée, on met l'error_code à 0 et on continue
            print_message(ERROR, "Ce nom d'utilisateur existe déjà.\n");
            error_code = 0;
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
 * Fonction permettant au client de se connecter au serveur quand il le demande
 * @return code d'erreur ou 0 si tout se passe bien
 */
int connect_server() {
    // Création de la requête
    json_object* request = create_request("connect");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "username",
            "password",
            NULL
    };
    printf("Connexion au serveur\n\n");
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
        case 0:
            cookie = json_object_get_int(json_object_object_get(result_params, "cookie"));
            print_message(SUCCESS, "Connexion réussie !\n");
            break;

        case 1:
            print_message(ERROR, "Ce nom d'utilisateur n'existe pas.\n");
            error_code = 0;
            break;

        case 2:
            print_message(ERROR, "Mot de passe incorrect.\n");
            error_code = 0;
            break;

        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;

}

int send_gazou(){

    // Création de la requête
    json_object* request = create_request("send_gazou");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    printf("Envoi d'un gazouilli\n\n");

    //Récupération du message tapé par l'utilisateur
    char buf[MAXDATASIZE];
    memset(buf, 0, MAXDATASIZE);

    //Si le message tapé est vide
    if (prompt_user_for_parameter("Gazouilli", buf) != 0) {
        print_message(ERROR, "Veuillez entrer un message non vide\n");
        return 1;
    }

    //Si le message tapé fait plus de 140 caractères
    if (strlen(buf) > 140){
        print_message(ERROR, "Veuillez entrer un message de moins de 140 caractères\n");
        return 2;
    }


    json_object* gazouilli = json_object_new_object();

    if (json_object_object_add(gazouilli, "content", json_object_new_string(buf)) != 0) {
        return 3;
    }

    json_object* list_of_tags = json_object_new_array();


    //Récupération des tags dans le buffer et ajout dans list_of_tags
    for (char* start_cursor = buf ; *start_cursor != '\0' ; start_cursor++){
        if ((*start_cursor) == '#'){
            char* end_cursor = start_cursor;
            while(*end_cursor != ' ' && *end_cursor != '\0'){
                end_cursor++;
            }
            if (*end_cursor == ' '){
                *end_cursor = '\0';
                json_object_array_add(list_of_tags, json_object_new_string(start_cursor));
                *end_cursor = ' ';
                start_cursor = end_cursor;
            } else {
                json_object_array_add(list_of_tags, json_object_new_string(start_cursor+1));
                start_cursor = end_cursor-1;
            }
        }
    }

    json_object_object_add(gazouilli, "list_of_tags", list_of_tags);
    char buffer[26] = {'\0'};
    get_iso_time_now(buffer);
    json_object_object_add(gazouilli, "date", json_object_new_string(buffer));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "gazouilli", gazouilli);
    json_object_object_add(params, "cookie", json_object_new_int(cookie));

    json_object_object_add(request, "params", params);

    //Envoie de la requête
    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    switch (error_code) {
        case 0:
            print_message(SUCCESS, "Message envoyé !\n");
            break;

        case 1:
            print_message(ERROR, "Message comportant un/des caractère(s) non supporté(s).\n");
            error_code = 0;
            break;

        case 2:
            print_message(ERROR, "Message trop long.\n");
            error_code = 0;
            break;

        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

int follow_user(){
    // Création de la requête
    json_object* request = create_request("follow_user");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "username",
            NULL
    };
    printf("Suivre un nouvel utilisateur :\n\n");
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
        case 0:
            print_message(SUCCESS, "Utilisateur suivi !\n");
            break;

        case 1:
            print_message(ERROR, "Le nom d'utilisateur que vous voulez suivre n'existe pas.\n");
            error_code = 0;
            break;

        case 2:
            print_message(ERROR, "Vous êtes déjà abonné à cet utilisateur.\n");
            error_code = 0;
            break;

        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;

}

int unfollow_user(){
    // Création de la requête
    json_object* request = create_request("unfollow_user");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "username",
            NULL
    };
    printf("Ne plus suivre un utilisateur :\n\n");
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
        case 0:
            print_message(SUCCESS, "Vous ne suivez plus l'utilisateur !\n");
            break;

        case 1:
            print_message(ERROR, "Vous n'êtes pas abonné à cet utilisateur.\n");
            error_code = 0;
            break;

        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;

}

int follow_tag(){
    // Création de la requête
    json_object* request = create_request("follow_tag");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "tag",
            NULL
    };
    printf("Suivre un nouveau tag :\n\n");
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
        case 0:
            print_message(SUCCESS, "Tag suivi !\n");
            break;

        case 1:
            print_message(ERROR, "Vous êtes déjà abonné à ce tag.\n");
            error_code = 0;
            break;

        default:
            print_message(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;

}

int unfollow_tag(){
    // Création de la requête
    json_object* request = create_request("unfollow_tag");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "tag",
            NULL
    };
    printf("Ne plus suivre un tag :\n\n");
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
        case 0:
            print_message(SUCCESS, "Vous ne suivez plus ce tag !\n");
            break;

        case 1:
            print_message(ERROR, "Vous n'êtes pas abonné à ce tag.\n");
            error_code = 0;
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
    cookie = -1;
    return 0;
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
    if (cookie != -1){
        json_object_object_add(params, "cookie", json_object_new_int(cookie));
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
    if (user_input > functions_count) {
        return not_implemented;
    } else {
        return functions[user_input];
    }
}
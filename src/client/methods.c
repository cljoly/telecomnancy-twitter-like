#include "methods.h"
#include "terminal.h"
#include "client.h"
#include "tools.h"
#include <json.h>
#include <limits.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

extern int cookie;
extern int sockfd;
extern char* username;

request_function functions[] = {
        quit,               // Quitter
        create_account,     // Créer un compte
        connect_server,     // Se connecter
        send_gazou,         // Envoyer un gazouilli
        get_gazou,          // Gazouillis reçus
        relay_gazou,        // Relayer un gazouilli
        follow_user,        // Suivre un utilisateur
        unfollow_user,      // Ne plus suivre un utilisateur
        list_followed_users,// Utilisateurs suivis
        list_my_followers,  // Mes Abonnés
        follow_tag,         // Suivre un tag
        unfollow_tag,       // Ne plus suivre un tag
        list_followed_tags, // Tags suivis
        disconnect          // Déconnexion
};
const unsigned int functions_count = sizeof(functions) / sizeof(request_function);

void handle_generic_error_code(int error_code) {
    switch (error_code) {
        case 10:
            print_message_above(FATAL_ERROR, "Un message non conforme a été envoyé au serveur.\n");
            break;
        case 11:
            print_message_above(ERROR, "Le serveur a rencontré un problème, veuillez réessayer plus tard.\n");
            break;
        case 13:
            print_message_above(ERROR, "Cette fonctionnalité n'est pas disponible sur le serveur.\n");
            break;
        case 14:
            cookie = -1;
            free(username);
            username = NULL;
            print_message_above(ERROR, "Erreur d'authentification. Vous avez été déconnecté, veuillez vous reconnecter.\n");
            break;
        case 12:
            // Code d'erreur réservé au client
        default:
            print_message_above(FATAL_ERROR, "Code d'erreur inconnu: %d\n.", error_code);
            exit(error_code);
    }
}


/**
 * Fonction de requête précisant que la commande voulue n'est pas encore implémentée
 * @return 1
 */
int not_implemented() {
    print_message_above(FATAL_ERROR, "Commande non implémentée");
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
    if( fill_request(request, params) != 0) {
        return 1;
    }
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
            print_message_above(SUCCESS, "Compte créé !\n");
            break;

        case 1: //erreur de notre doc : elle est gérée, on met l'error_code à 0 et on continue
            print_message_above(ERROR, "Ce nom d'utilisateur existe déjà.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
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


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    switch (error_code) {
        case 0:
            cookie = json_object_get_int(json_object_object_get(result_params, "cookie"));
            print_message_above(SUCCESS, "Connexion réussie !\n");

            // Récupération du nom de l'utilisateur
            const json_object* params_json = json_object_object_get(request, "params");
            const char* username_params = json_object_to_json_string(json_object_object_get(params_json, "username"));
            const size_t username_params_len = strnlen(username_params, MAXDATASIZE);

            if(username_params[0] == '"' && username_params[username_params_len-1] == '"') {
                username = calloc(username_params_len-1, sizeof(char));
                strncpy(username, username_params+1, strlen(username_params)-2);
            } else {
                username = calloc(username_params_len+1, sizeof(char));
                strncpy(username, username_params, MAXDATASIZE);
            }

            break;

        case 1:
            print_message_above(ERROR, "Ce nom d'utilisateur n'existe pas.\n");
            error_code = 0;
            break;

        case 2:
            print_message_above(ERROR, "Mot de passe incorrect.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
            break;
    }

    // free du résultat et de la requête
    json_object_put(request);
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
    if (prompt_user_for_parameter("Gazouilli", buf, 0) != 0) {
        print_message_above(ERROR, "Veuillez entrer un message non vide\n");
        return 1;
    }

    //Si le message tapé fait plus de 140 caractères
    if (strlen(buf) > 140){
        print_message_above(ERROR, "Veuillez entrer un message de moins de 140 caractères\n");
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
                json_object_array_add(list_of_tags, json_object_new_string(start_cursor+1));
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
            print_message_above(SUCCESS, "Message envoyé !\n");
            break;

        case 1:
            print_message_above(ERROR, "Message comportant un/des caractère(s) non supporté(s).\n");
            error_code = 0;
            break;

        case 2:
            print_message_above(ERROR, "Message trop long.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
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
            print_message_above(SUCCESS, "Utilisateur suivi !\n");
            break;

        case 1:
            print_message_above(ERROR, "Le nom d'utilisateur que vous voulez suivre n'existe pas.\n");
            error_code = 0;
            break;

        case 2:
            print_message_above(ERROR, "Vous êtes déjà abonné à cet utilisateur.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
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
            print_message_above(SUCCESS, "Vous ne suivez plus l'utilisateur !\n");
            break;

        case 1:
            print_message_above(ERROR, "Vous n'êtes pas abonné à cet utilisateur.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
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
            print_message_above(SUCCESS, "Tag suivi !\n");
            break;

        case 1:
            print_message_above(ERROR, "Vous êtes déjà abonné à ce tag.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
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
            print_message_above(SUCCESS, "Vous ne suivez plus ce tag !\n");
            break;

        case 1:
            print_message_above(ERROR, "Vous n'êtes pas abonné à ce tag.\n");
            error_code = 0;
            break;

        default:
            handle_generic_error_code(error_code);
            error_code = 0;
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;

}

int list_followed_users(){
    // Création de la requête
    json_object* request = create_request("list_followed_users");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "cookie", json_object_new_int(cookie));
    json_object_object_add(request, "params", params);


    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    if (error_code == 0) {
            json_object* followed_users_json = json_object_object_get(result_params, "list_of_users");
            array_list* list_of_followed_users = json_object_get_array(followed_users_json);

            if(array_list_length(list_of_followed_users) == 0) {
                print_message_below(SUCCESS, "Aucun utilisateur suivi.\n");
            } else {
                print_message_below(SUCCESS, "Liste des utilisateurs suivis :\n");
                for (size_t i = 0; i < array_list_length(list_of_followed_users); i++) {
                    json_object* item = array_list_get_idx(list_of_followed_users, i);
                    const char* followed = json_object_get_string(item);
                    printf("%s\n", followed);
                }
            }

    } else {
        handle_generic_error_code(error_code);
        error_code = 0;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

int list_followed_tags(){
    // Création de la requête
    json_object* request = create_request("list_followed_tags");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "cookie", json_object_new_int(cookie));
    json_object_object_add(request, "params", params);


    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    if (error_code == 0) {
            json_object* followed_tags_json = json_object_object_get(result_params, "list_of_tags");
            array_list* list_of_followed_tags = json_object_get_array(followed_tags_json);

            if(array_list_length(list_of_followed_tags) == 0) {
                print_message_below(SUCCESS, "Aucun tag suivi.\n");
            } else {
                print_message_below(SUCCESS, "Liste des tags suivis :\n");
                for (size_t i = 0; i < array_list_length(list_of_followed_tags); i++) {
                    json_object* item = array_list_get_idx(list_of_followed_tags, i);
                    const char* followed = json_object_get_string(item);
                    printf("%s\n", followed);
                }
            }
    } else {
        handle_generic_error_code(error_code);
        error_code = 0;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

int list_my_followers(){
    // Création de la requête
    json_object* request = create_request("list_my_followers");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "cookie", json_object_new_int(cookie));
    json_object_object_add(request, "params", params);


    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    if (error_code == 0) {
        json_object* followers_users_json = json_object_object_get(result_params, "list_of_followers");
        array_list* list_of_followers_users = json_object_get_array(followers_users_json);

        if(array_list_length(list_of_followers_users) == 0) {
            print_message_below(SUCCESS, "Aucun abonné.\n");
        } else {
            print_message_below(SUCCESS, "Liste de mes abonnés :\n");
            for (size_t i = 0; i < array_list_length(list_of_followers_users); i++) {
                json_object* item = array_list_get_idx(list_of_followers_users, i);
                const char* followed = json_object_get_string(item);
                printf("%s\n", followed);
            }
        }
    }else {
        handle_generic_error_code(error_code);
        error_code = 0;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

int get_gazou(){
    // Création de la requête
    json_object* request = create_request("get_gazou");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "nb_gazou", json_object_new_int(NUMBER_OF_GAZOU));
    json_object_object_add(params, "cookie", json_object_new_int(cookie));
    json_object_object_add(request, "params", params);


    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    if (error_code == 0) {
        json_object* gazous_json = json_object_object_get(result_params, "list_of_gazous");
        array_list* list_of_gazous = json_object_get_array(gazous_json);

        if (array_list_length(list_of_gazous) == 0) {
            print_message_below(SUCCESS, "Aucun gazouilli reçu.\n");
        } else {
            print_message_below(SUCCESS, "Liste des gazouillis reçus :\n");
            for (size_t i = 0; i < array_list_length(list_of_gazous); i++) {
                json_object* gazou_json = array_list_get_idx(list_of_gazous, i);
                print_gazou(gazou_json);
            }
        }
    } else {
        handle_generic_error_code(error_code);
        error_code = 0;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

void print_gazou(json_object* gazou_json){
    int id = json_object_get_int(json_object_object_get(gazou_json, "id"));
    const char* author = json_object_get_string(json_object_object_get(gazou_json, "author"));
    printf("\033[2m%d - de %s\033[0m", id, author);

    json_object* retweeter_json;
    if (json_object_object_get_ex(gazou_json, "retweeter", &retweeter_json)) {
        const char* relay_author = json_object_get_string(retweeter_json);
        if (relay_author[0] != '\0') {
            printf(", relayé par %s", relay_author);
        }
    }
    printf("\n");


    const char* content = json_object_get_string(json_object_object_get(gazou_json, "content"));
    printf("%s\n", content);


    const char* date = json_object_get_string(json_object_object_get(gazou_json, "date"));
    //TODO: voir avec Clément
    /*json_object* retweet_date_json;
    if (json_object_object_get_ex(gazou_json, "retweet_date", &retweet_date_json)) {
        date = json_object_get_string(retweet_date_json);
    }*/

    printf("\033[3mLe %.*s à %s\033[0m\n", 10, date, date+11);
    printf("\n");
}

int relay_gazou(){
    // Création de la requête
    json_object* request = create_request("get_gazou");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));

    json_object* params = json_object_new_object();
    json_object_object_add(params, "nb_gazou", json_object_new_int(NUMBER_OF_GAZOU));
    json_object_object_add(params, "cookie", json_object_new_int(cookie));
    json_object_object_add(request, "params", params);


    if (send_message(json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(request_id, &result_params);
    if (error_code == 0) {
        json_object* gazous_json = json_object_object_get(result_params, "list_of_gazous");
        array_list* list_of_gazous = json_object_get_array(gazous_json);

        if (array_list_length(list_of_gazous) == 0) {
            print_message_below(SUCCESS, "Aucun gazouilli relayable.\n");
        } else {
            print_message_below(SUCCESS, "Liste des gazouillis relayables :\n");
            for (size_t i = 0; i < array_list_length(list_of_gazous); i++) {
                json_object* gazou_json = array_list_get_idx(list_of_gazous, i);
                print_gazou(gazou_json);
            }
        }

        // Création de la requête
        json_object* request2 = create_request("relay_gazou");
        const unsigned int request2_id = (unsigned int) json_object_get_int(json_object_object_get(request2, "id"));


        printf("\033[1;1H");
        printf("Relayer un gazouilli\n\n");
        clear_above_below_positions();

        char buf[MAXDATASIZE];
        json_object* params_relay = json_object_new_object();
        memset(buf, 0, MAXDATASIZE);
        if (prompt_user_for_parameter("ID du gazouilli à relayer", buf, 0) != 0) {
            return 1;
        }
        const unsigned int requested_gazou_id = string_to_unsigned_int(buf);
        int requested_gazou_is_valid = 0 ;
        for (size_t i = 0; i < array_list_length(list_of_gazous); i++) {
            json_object* gazou_json = array_list_get_idx(list_of_gazous, i);
            int gazou_id = json_object_get_int(json_object_object_get(gazou_json, "id"));
            if( gazou_id == (int)requested_gazou_id) {
                requested_gazou_is_valid = 1;
                break;
            }
        }
        if( ! requested_gazou_is_valid ) {
            print_message_above(ERROR, "Veuillez entrer ID de gazou valide.\n");
            return 0;
        }
        json_object_object_add(params_relay, "id_gazouilli", json_object_new_int(requested_gazou_id));
        char buffer[26] = {'\0'};
        get_iso_time_now(buffer);
        json_object_object_add(params_relay, "retweet_date", json_object_new_string(buffer));

        if (cookie != -1) {
            json_object_object_add(params_relay, "cookie", json_object_new_int(cookie));
        }
        if (json_object_object_add(request2, "params", params_relay) != 0) {
            return 3;
        }

        if (send_message(json_object_to_json_string(request2)) != 0) {
            return 1;
        }
        // free de la requête
        json_object_put(request2);

        // Lecture et gestion de la réponse
        json_object* result2_params = NULL;
        int error2_code = get_response_result(request2_id, &result2_params);
        switch (error2_code) {
            case 0:
                print_message_above(SUCCESS, "Gazouilli relayé !\n");
                break;

            case 1:
                print_message_above(ERROR, "ID du gazouilli invalide\n");
                error_code = 0;
                break;

            case 2:
                print_message_above(ERROR, "Gazouilli déjà relayé\n");
                error_code = 0;
                break;

            default:
                handle_generic_error_code(error_code);
                error_code = 0;
                break;
        }

        // free du résultat
        json_object_put(result_params);

    } else {
        // Erreur pour la liste des gazous
        handle_generic_error_code(error_code);
        error_code = 0;
    }
    clear_terminal_except_header();
    return error_code;
}


/**
 * Ferme la socket et quitte le programme.
 * TODO: déconnexion propre
 * @return
 */
int disconnect() {
    cookie = -1;
    print_title();
    free(username);
    username = NULL;
    return 0;
}

/**
 * Ferme la socket et quitte le programme
 * @return
 */
int quit(){
    clear_all_terminal();
    printf("Fermeture du programme et de la connexion.\n");
    free(username);
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
        int valid_param = 0;

        // Tant que le paramètre demandé n'est pas valide
        while (valid_param == 0) {
            // On le redemande

            // Si le contenu est vide
            if(prompt_user_for_parameter(params_name[i], buf, i) != 0) {
                print_message_above(ERROR, "Veuillez entrer une valeur pour le champ %s.\n", params_name[i]);
                memset(buf, 0, MAXDATASIZE);
            }
            // Dans le cas d'un username
            else if(strcmp(params_name[i], "username") == 0 ) {
                // S'il ne commence pas par un '@', il n'est pas valide
                if(buf[0] != '@' || strlen(buf) == 1) {
                    print_message_above(ERROR, "Un nom d'utilisateur doit toujours commencer par '@'\n");
                    memset(buf, 0, MAXDATASIZE);
                } else {
                    valid_param = 1;
                }
            } else {
                valid_param = 1;
            }
        }
        if (json_object_object_add(params, params_name[i], json_object_new_string(buf)) != 0) {
            return 2;
        }
    }
    clear_above_messages();
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
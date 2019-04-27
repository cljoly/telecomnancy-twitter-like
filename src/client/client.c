#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "client.h"

#include <json.h>

#define TERM_WIDTH 120

int connected = 0;

/**
 * Fonction de requête précisant que la commande voulue n'est pas encore implémentée
 * @return 1
 */
int not_implemented() {
    fprintf(stderr, "Commande non implémentée\n");
    return 1;
}

int create_account(int sockfd) {
    // Création de la requête
    json_object* request = create_request("create_account");
    const unsigned int request_id = (unsigned int) json_object_get_int(json_object_object_get(request, "id"));
    const char* params[] = {
            "username",
            "password",
            NULL
    };
    fill_request(request, params);
    if (send_message(sockfd, json_object_to_json_string(request)) != 0) {
        return 1;
    }
    // free de la requête
    json_object_put(request);


    // Lecture et gestion de la réponse
    json_object* result_params = NULL;
    int error_code = get_response_result(sockfd, request_id, &result_params);
    switch (error_code) {
        case 1: //erreur de notre doc : elle est gérée, on met l'error_code à 0 et on continue
            printf("Ce nom d'utilisateur existe déjà.\n");
            error_code = 0;
            break;
        case 0:
            printf("Compte créé!\n");
            break;
        default:
            fprintf(stderr, "Code d'erreur inconnu: %d\n.", error_code);
            break;
    }

    // free du résultat
    json_object_put(result_params);
    return error_code;
}

/**
 * Ferme la socket et quitte le programme.
 * TODO: déconnexion propre
 * @param sockfd_ptr
 * @return
 */
int disconnect(int sockfd) {
    close(sockfd);
    exit(0);
}


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


void clearTerminal() {
    printf("\e[1;1H\e[2J");
}

void print_menu() {
    clearTerminal();


    unsigned int first_command_index = 0;
    unsigned int last_command_index = 1;
    if (connected) {
        first_command_index = 2;
        last_command_index = commands_count - 1;
    }

    unsigned int printed_line_chars = 0;
    unsigned int longest_line_chars = 0;
    for (unsigned int i = first_command_index; i <= last_command_index; i++) {
        printed_line_chars += 5 + strlen(commands[i]);
        if (printed_line_chars > longest_line_chars) {
            longest_line_chars = printed_line_chars;
        }
        printf("%2d - %s", i - connected, commands[i]);

        // S'il faut encore afficher une commande
        if (i + 1 < commands_count) {
            // S'il y a la place pour l'afficher
            if (printed_line_chars + 4 + 5 + strlen(commands[i + 1]) < TERM_WIDTH) {
                printf("\t");

                // Ajout de la longueur de la tabulation
                while (printed_line_chars % 4 != 0) {
                    printed_line_chars++;
                }
            } else {
                // Sinon, nouvelle ligne
                printf("\n");
                printed_line_chars = 0;
            }
        }
    }
    printf("\n");
    // print a line
    for (unsigned int j = 0; j < longest_line_chars; j++) {
        printf("-");
    }
    printf("\n\n");
}

/**
 * Demande à l'utilisateur l'action à effectuer et la retourne
 * Affiche le menu, en appellant \seealso print_menu()
 * @return le numéro de la commande demandée
 */
unsigned int prompt_user() {
    print_menu();
    // prompt
    printf("> Quelle action voulez-vous effectuer ? ");

    // Lecture des données
    char buf[3] = {'\0'};
    scanf("%s", buf);

    // conversion et test
    char* endptr;
    unsigned int input = (unsigned int) strtoul(buf, &endptr, 10);
    if (endptr == buf) {
        return -1; //TODO : trouver mieux que -1 ?
    } else if (input > commands_count) {
        fprintf(stderr, "Commande invalide\n");
        return -1;
    } else {
        return input;
    }
}

/**
 * Demande la valeur d'un paramètre d'une requête à l'utilisateur
 * @param prompt Le message affiché pour expliquer la demande
 * @param result L'entrée tappée par l'utilisateur
 * @return 1 en cas d'erreur, 0 sinon
 */
int prompt_user_for_parameter(const char* prompt, char* result) {
    printf("%s : ", prompt);
    int scanned_items = scanf("%s", result);
    if (scanned_items != 1) {
        return 1;
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

/**
 * Initialise la connection au serveur donné
 * @param server structure du serveur
 * @param server_port port du serveur
 * @return le descripteur de fichier de la socket à utiliser
 */
int init_connection(const struct hostent* server, int server_port) {
    //socket file descriptor
    int sockfd = -1;

    //connexion IPv4 (AF_INET), TCP (SOCK_STREAM)
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Erreur création socket file descriptor\n");
        exit(3);
    }

    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(struct sockaddr_in));

    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(server_port);
    server_info.sin_addr = *((struct in_addr*) server->h_addr);


    if (connect(sockfd, (struct sockaddr*) &server_info, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Erreur de connect\n");
        exit(4);
    }
    return sockfd;
}

/**
 * Envoie le message donné sur la socket donnée
 * @param sockfd Descripteur de fichier de la socket
 * @param message Message à envoyer
 * @return 1 en cas d'erreur, 0 sinon
 */
int send_message(int sockfd, const char* message) {
    if (send(sockfd, message, strlen(message), 0) == -1) {
        fprintf(stderr, "Erreur envoi message: %s", strerror(errno));
        return 1;
    }
    return 0;
}

/**
 * Récupère et lit la réponse du serveur.
 * @param sockfd Le descripteur de fichier de la socket utilisée pour échanger avec le serveur
 * @param buf Le buffer pour récupérer la réponse
 * @return NULL en cas d'erreur, la réponse sinon.
 */
json_object* get_response_object(int sockfd) {
    json_object* response = NULL;
    json_tokener* tokener = json_tokener_new();
    enum json_tokener_error error = json_tokener_success;

    // On lit la réponse tant qu'elle n'est pas complète,
    // c'est à dire que le JSON détecte qu'il n'est pas complet
    do {
        char buf[MAXDATASIZE] = {'\0'};
        ssize_t receive_data_size = -1;

        // Lecture du message
        if ((receive_data_size = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            fprintf(stderr, "Erreur réception message: %s\n", strerror(errno));
            return NULL;
        }
        buf[receive_data_size] = '\0';

        // parsing et récupération des erreurs éventuelles
        response = json_tokener_parse_ex(tokener, buf, receive_data_size);
        error = json_tokener_get_error(tokener);

    } while (response == NULL && error == json_tokener_continue);

    return response;
}

int check_response(json_object* response, unsigned int request_id) {
    unsigned int response_id = (unsigned int) json_object_get_int(json_object_object_get(response, "id"));
    if (request_id != response_id) {
        fprintf(stderr, "Erreur: attente de la réponse %d, reçue %d", request_id, response_id);
    }


    json_object* error = json_object_object_get(response, "error");
    if (error != NULL) {
        const char* result_method = json_object_get_string(error);
        int error_code = json_object_get_int(json_object_object_get(response, "error_code"));
        fprintf(stderr, "Erreur: Message %d: Méthode %s, code: %d", response_id, result_method, error_code);
        return error_code;
    }
    return 0;
}

int get_response_result(int sockfd, unsigned int id, json_object** result) {
    json_object* response = get_response_object(sockfd);
    int error_code = check_response(response, id);
    if (error_code != 0) {
        *result = NULL;
        return error_code;
    } else {
        *result = json_object_object_get(response, "params");
        return 0;
    }
}


int main(int argc, char* argv[]) {

    unsigned int srv_port = DEFAULT_PORT;

    if (argc < 2) {
        usage();
        exit(1);
    } else if (argc == 3){
        char* endptr;
        srv_port = (unsigned int) strtoul(argv[2], &endptr, 10);
        if (endptr == argv[2]) {
            usage();
            exit(1);
        }
    }

    struct hostent* server = NULL;
    if ((server = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "Erreur gethostbyname\n");
        exit(2);
    }


    int sockfd = init_connection(server, srv_port);


    while (1) {
        // récupération de la commande utilisateur
        unsigned int command = prompt_user();
        request_function function = get_function(command);

        // Appel de la fonction
        int return_code = function(sockfd);
        if (return_code != 0) {
            exit(return_code);
        }
    }

    close(sockfd);

    return 0;
}


void usage() {
    printf("usage : client IP_du_serveur [port_du_serveur]\n");
}
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

#define TERM_WIDTH 140

/**
 * Fonction de requête précisant que la commande voulue n'est pas encore implémentée
 * @return 1
 */
int not_implemented() {
    fprintf(stderr, "Commande non implémentée\n");
    return 1;
}

int test_hello(void* sockfd_ptr) {
    if (send(*(int*)sockfd_ptr, "Hello, world!\n", 15, 0) == -1) {
        fprintf(stderr, "Erreur envoi message\n");
        return 1;
    }
    return 0;
}

/**
 * Ferme la socket et quitte le programme.
 * TODO: déconnexion propre
 * @param sockfd_ptr
 * @return
 */
int disconnect(void* sockfd_ptr) {
    close(*(int*)sockfd_ptr);
    exit(0);
}


void clearTerminal() {
    printf("\033[2J");
}

void print_menu() {
    clearTerminal();

    unsigned int printed_line_chars = 0;
    for (unsigned int i = 0; i < commands_count; i++) {
        printed_line_chars += 5 + strlen(commands[i]);
        printf("%2d - %s", i, commands[i]);

        // S'il faut encore afficher une commande
        if (i + 1 < commands_count) {
            // S'il y a la place pour l'afficher
            if (printed_line_chars + strlen(commands[i + 1]) + 5 < TERM_WIDTH) {
                printf("\t");
            } else {
                // Sinon, nouvelle ligne
                printf("\n");
                printed_line_chars = 0;
            }
        }
    }
    printf("\n");
    // print a line
    for (int j = 0; j < TERM_WIDTH; j++) {
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
    char buf[3];
    scanf("%s", buf);

    // conversion et test
    char* endptr;
    unsigned int input = strtoul(buf, &endptr, 10);
    if( endptr == buf) {
        return -1;
    }

    if( input > commands_count ) {
        fprintf(stderr, "Commande invalide\n");
        return -1;
    }
    return input;
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

/**
 * Récupère et lit la réponse du serveur.
 * @param sockfd Le descripteur de fichier de la socket utilisée pour échanger avec le serveur
 * @param buf Le buffer pour récupérer la réponse
 * @return
 */
char* get_response(int sockfd, char* buf, size_t bufsize) {
    ssize_t receive_data_size = -1;

    if ((receive_data_size = recv(sockfd, buf, bufsize, 0)) == -1) {
        fprintf(stderr, "Erreur réception message \n");
        exit(6);
    }

    buf[receive_data_size] = '\0';
    return buf;
}

/**
 * Initialise la connection au serveur donné
 * @param server
 * @return le descripteur de fichier de la socket à utiliser
 */
int init_connection(struct hostent* server) {
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
    server_info.sin_port = htons(DEFAULT_PORT);
    server_info.sin_addr = *((struct in_addr*) server->h_addr);


    if (connect(sockfd, (struct sockaddr*) &server_info, sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Erreur de connect\n");
        exit(4);
    }
    return sockfd;
}


int main(int argc, char* argv[]) {

    if (argc != 2) {
        usage();
        exit(1);
    }

    struct hostent* server = NULL;
    if ((server = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "Erreur gethostbyname\n");
        exit(2);
    }


    int sockfd = init_connection(server);



    while (1) {
        // récupération de la commande utilisateur
        int command = prompt_user();
        request_function function = get_function(command);

        // Appel de la fonction
        if( function(&sockfd) != 0) {
            // Exit in case of error
            break;
        }


        char buf[MAXDATASIZE] = {'\0'};
        get_response(sockfd, buf, MAXDATASIZE);

        printf("Received text='%s'\n", buf);
    }

    close(sockfd);

    return 0;
}


void usage() {
    printf("usage : client IP_du_serveur\n");
}
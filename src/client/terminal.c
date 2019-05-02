#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <limits.h>

#include "client.h"
#include "tools.h"
#include "terminal.h"

#define HEADER_HEIGHT 9
static int prompt_line = HEADER_HEIGHT+1;

static char* commands[] = {
        "Quitter",
        "Créer un compte",
        "Se connecter",
        "Envoyer un gazouilli",
        "Gazouillis reçus",
        "Relayer un gazouilli",
        "Suivre un utilisateur",
        "Ne plus suivre un utilisateur",
        "Utilisateurs suivis",
        "Mes Abonnés",
        "Suivre un tag",
        "Ne plus suivre un tag",
        "Tags suivis",
        "Déconnexion"
};

static const unsigned int commands_count = sizeof(commands) / sizeof(char*);

static int above_count = 0;
static int below_count = 0;

extern char* username;

void clear_above_below_positions() {
    if( above_count != 2) {
        above_count = 0;
    }
    below_count = 0;
}

/**
 * Efface l'entièreté du terminal, et place le curseur du print au début de la première ligne
 */
void clear_all_terminal() {
    clear_above_below_positions();
    printf("\e[1;1H\e[2J");
}

/**
 * Efface le terminal en concervant le header, et place le curseur du print au début de la ligne 9
 */
void clear_terminal_except_header() {
    clear_above_below_positions();
    printf("\033[%d;1H", HEADER_HEIGHT);
}

/**
 * Affiche le menu des commandes possibles en fonction de l'état de l'utilisateur
 * @param first_command_index L'indice de la première commande à afficher
 * @param last_command_index  L'indice de la dernière commande à afficher
 */
void print_menu(unsigned int first_command_index, unsigned int last_command_index) {
    clear_terminal_except_header();

    // get terminal info
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // print a line
    for (unsigned int j = 0; j < w.ws_col; j++) {
        printf("-");
    }
    printf("\n");
    prompt_line = HEADER_HEIGHT+1;

    unsigned int printed_line_chars = 0;

    // On récupère la taille de la plus grande commande
    unsigned int longest_command = 0;
    for (unsigned int i = first_command_index; i <= last_command_index; i++) {
        const size_t command_length = strlen(commands[i]);
        if(command_length > longest_command) {
            longest_command = command_length;
        }
    }

    // On compte les caractères à afficher pour une commande (numéro + ' - ' + longueur de commande)
    const unsigned int entry_length = 2 + 3 + longest_command;

    for (unsigned int i = first_command_index; i <= last_command_index; i++) {
        // Affichage de la commande
        printf("%2d - %s", i , commands[i]);


        // Tout le reste du bloc concerne la gestion de l'indentation dynamique et des sauts de ligne

        // S'il reste au moins une commande à afficher
        if( i+1 <= last_command_index) {

            // Si on ne peut pas afficher la prochaine commande ( caractères déjà affichés + commande i et espaces + prochaine commande)
            if (w.ws_col == 0 || printed_line_chars + entry_length + strlen(commands[i+1]) >= w.ws_col) {
                printf("\n");
                prompt_line++;
                printed_line_chars = 0;
            } else {
                // On "complète" la commande avec des espaces pour obtenir une indentation régulière
                unsigned int j = strlen(commands[i]);
                while(j <= longest_command && printed_line_chars + j < w.ws_col) {
                    printf(" ");
                    j++;
                }

                printed_line_chars += 2 + 3 + j;

                // 4 espaces supplémentaires entre chaque commande
                if(printed_line_chars + j + 4 < w.ws_col) {
                    printf("    ");
                    printed_line_chars += 4;
                } else {
                    printf("\n");
                    prompt_line++;
                    printed_line_chars = 0;
                }
            }
        }
    }

    printf("\n");
    prompt_line++;
    // print a line
    for (unsigned int j = 0; j < w.ws_col; j++) {
        printf("-");
    }
    printf("\n\n");

    prompt_line+=2;
}

/**
 * Affiche un message en couleur, en fonction du type donné.
 * Cette fonction utilise printf() et fonctionne de manière identique pour les paramètres autres que type.
 * @param type Type de message
 * @param format Format du message (comme pour printf)
 * @param ... Paramètres éventuels de format (comme pour printf)
 */
void print_message(message_type_t type, const char* format, va_list args) {
    switch(type) {
        case SUCCESS:
            // print in green
            printf("\033[1;32m");
            break;
        case ERROR:
            // print in yellow
            printf("\033[1;31m");
            break;
        case FATAL_ERROR:
            // print in red
            printf("\033[1;31m");
            break;
        case DEBUG:
            // print in grey
            printf("\033[1;35m");
            break;
        case INFO:
        default:
            // print in blue
            printf("\033[0;34m");
            break;
    }

    vfprintf(stdout, format, args);
    printf("\033[0m");
}

/**
 * Affiche un message au dessus du menu (à partir de la ligne 8)
 * @see print_message
 * @param type Type de message
 * @param format Format du message (comme pour printf)
 * @param ... Paramètres éventuels de format (comme pour printf)
 */
void print_message_above(message_type_t type, const char* format, ...) {
    printf("\033[%d;1H", (5+above_count));
    above_count++;
    if( above_count > 9) {
        above_count=5;
    }

    va_list args;
    va_start(args, format);
    print_message(type, format, args);
    va_end(args);
}

/**
 * Affiche un message en dessous du menu (à partir de la ligne 16)
 * @see print_message
 * @param type Type de message
 * @param format Format du message (comme pour printf)
 * @param ... Paramètres éventuels de format (comme pour printf)
 */
void print_message_below(message_type_t type, const char* format, ...) {
    printf("\033[%d;1H", prompt_line+2+below_count);
    below_count++;

    va_list args;
    va_start(args, format);
    print_message(type, format, args);
    va_end(args);
}

/**
 * Demande à l'utilisateur l'action à effectuer et la retourne
 * Affiche le menu, en appellant \seealso print_menu()
 * @param cookie Le cookie courant de l'utilisateur ou -1 si l'utilisateur n'et pas connecté
 * @return le numéro de la commande demandée
 */
unsigned int prompt_user(int cookie) {
    unsigned int first_command_index = 0;
    unsigned int last_command_index = 2;
    if (cookie != -1) {
        first_command_index = 3;
        last_command_index = commands_count - 1;
    }
    print_menu(first_command_index, last_command_index);
    // prompt
    if(cookie != -1) {
        printf("\033[%d;1H", prompt_line);
        printf("\033[0;34m");
        printf("Connecté en tant que %s.", username);
        printf("\033[0m");
    }
    printf("\033[%d;1H", prompt_line+1);
    printf("\n");
    printf("\033[%d;1H", prompt_line+1);
    printf("> Quelle action voulez-vous effectuer ? ");

    // Lecture des données
    char buf[3] = {'\0'};
    read_stdin(buf, 3);

    // conversion et test
    char* endptr;
    unsigned int input = (unsigned int) strtoul(buf, &endptr, 10);
    if (endptr == buf) {
        print_message_above(ERROR, "Veuillez entrer un numéro de commande\n");
        return UINT_MAX;
    } else if (input < first_command_index || input > last_command_index) {
        print_message_above(ERROR, "Veuillez entrer une commande valide\n");
        return UINT_MAX;
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
    size_t length = read_stdin(result, MAXDATASIZE);
    //Si la longueur vaut 0, c'est une erreur
    return length == 0;
}

/**
 * Affiche MyTwitter ave un ASCII art de 6 lignes
 */
void print_title() {
    printf("\033[0;36m");
    printf(" __  __      _____          _ _   _\n");
    printf("|  \\/  |_   |_   _|_      _(_) |_| |_ ___ _ __\n");
    printf("| |\\/| | | | || | \\ \\ /\\ / / | __| __/ _ \\ '__|\n");
    printf("| |  | | |_| || |  \\ V  V /| | |_| ||  __/ |\n");
    printf("|_|  |_|\\__, ||_|   \\_/\\_/ |_|\\__|\\__\\___|_|\n");
    printf("        |___/\n");
    printf("\033[0m");
    above_count = 2;
    below_count = 0;
}
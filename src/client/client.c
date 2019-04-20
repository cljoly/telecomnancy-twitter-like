#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.h"


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

    ssize_t receive_data_size = -1;
    char buf[MAXDATASIZE] = {'\0'};

    while (1) {
        if (send(sockfd, "Hello, world!\n", 15, 0) == -1) {
            fprintf(stderr, "Erreur envoi message \n");
            exit(5);
        }

        printf("Message bien envoyé\n");

        if ((receive_data_size = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            fprintf(stderr, "Erreur réception message \n");
            exit(6);
        }

        buf[receive_data_size] = '\0';

        printf("Received text='%s'\n", buf);
        sleep(1);

    }

    close(sockfd);

    return 0;
}


void usage() {
    printf("usage : client IP_du_serveur\n");
}
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"

void usage() { printf("usage : servmulti numero_port_serveur\n"); }

int main(int argc, char *argv[]) {
  int sockfd;
  int newsockfd;
  int childpid;
  int tab_clients[FDSET_SIZE];
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  /* Verifier le nombre de paramètre en entrée */
  if (argc != 2) {
    usage();
    exit(1);
  }

  printf("Démarrage du serveur\n");

  // Ouvrir une socket (a TCP socket)
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("servmulti : Probleme socket\n");
    exit(2);
  }
  fprintf(stderr, "Socket créée\n");

  // Lier l'adresse  locale à la socket
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("servmulti : erreur bind\n");
    exit(1);
  }
  fprintf(stderr, "Socket attachée\n");

  // Paramètrer le nombre de connexion "pending"
  if (listen(sockfd, SOMAXCONN) < 0) {
    perror("servmulti : erreur listen\n");
    exit(1);
  }
  fprintf(stderr, "listen, max %i\n", SOMAXCONN);

  int maxfdp1 = sockfd + 1;

  fd_set pset;
  fd_set rset;

  FD_ZERO(&pset);
  FD_ZERO(&rset);
  FD_SET(sockfd, &rset);

  for (;;) {
    fprintf(stderr, "for ----\n");
    pset = rset;
    int nbfd = select(maxfdp1, &pset, NULL, NULL, NULL);
    fprintf(stderr, "select: %i\n", nbfd);
    if (nbfd <0) {
      perror("Main.c: erreur select");
    }

    if (FD_ISSET(sockfd, &pset)) {
      clilen = sizeof(cli_addr);
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      fprintf(stderr, "accept, socket: %i\n", newsockfd);
      if (newsockfd < 0) {
        perror("servmulti : erreur accept\n");
        exit(1);
      }

      // Recherche d’une place libre dans le tableau
      int i = 0;
      while ((i < FD_SETSIZE) && (tab_clients[i] >= 0)) i++;
      if (i == FD_SETSIZE) {
        exit(1);
      }
      fprintf(stderr, "place libre: %i\n", i);

      // Ajout du nouveau client au tableau des clients
      tab_clients[i] = newsockfd;

      // Ajout du nouveau client dans rset
      FD_SET(newsockfd, &rset);

      // Positionner maxfdp1
      if (newsockfd >= maxfdp1) {
        maxfdp1 = newsockfd + 1;
        fprintf(stderr, "maxfdp1 mis à jour: %i\n", maxfdp1);
      }
      nbfd--;
    }

    // Parcourir le tableau des clients connectés
    int i = 0;
    while ((nbfd > 0) && (i < FD_SETSIZE)) {
      if ((childpid = fork()) < 0) {
        perror("server: fork error\n");
        exit(1);
      } else if (childpid == 0) {
        close(sockfd);
        while (dispatch_request(newsockfd)) {}
        exit(0);
      }
    }

    close(newsockfd);
  }
}

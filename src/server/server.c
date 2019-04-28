#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <json.h>
#include <sqlite3.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"
#include "db.h"

void usage() { printf("usage : servmulti numero_port_serveur\n"); }

int main(int argc, char *argv[]) {
  int sockfd=3;
  int newsockfd;
  int childpid;
  int tab_clients[FDSET_SIZE_CLIENT];
  memset(tab_clients, -1, sizeof(tab_clients));
  struct sockaddr_in6 serv_addr;
  struct sockaddr_in6 cli_addr;
  socklen_t clilen;

  /* Définition du port d’entrée */
  int srv_port = DEFAULT_PORT;
  if (argc != 2) {
    usage();
  } else {
    srv_port = atoi(argv[1]);
  }

  printf("Démarrage du serveur, port %i, pid %i\n", srv_port, getpid());

  // Ouvrir une socket (a TCP socket)
  if ((sockfd = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("servmulti : Probleme socket");
    exit(2);
  }
  printf("Socket créée\n");

  // Lier l'adresse  locale à la socket
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin6_family = PF_INET6;
  //serv_addr.sin6_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin6_port = htons(srv_port);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("servmulti : erreur bind");
    exit(3);
  }
  printf("Socket attachée\n");

  // Paramètrer le nombre de connexion "pending"
  if (listen(sockfd, SOMAXCONN) < 0) {
    perror("servmulti : erreur listen");
    exit(4);
  }
  printf("listen, max %i\n", SOMAXCONN);

  int maxfdp1 = sockfd + 1;

  fd_set pset;
  fd_set rset;

  FD_ZERO(&pset);
  FD_ZERO(&rset);
  FD_SET(sockfd, &rset);

  for (;;) {
    pset = rset;
    printf("select bloquant…\n");
    int nbfd = select(maxfdp1, &pset, NULL, NULL, NULL);
    printf("select: %i\n", nbfd);
    if (nbfd <0) {
      perror("Main.c: erreur select");
    }

    if (FD_ISSET(sockfd, &pset)) { // Enregistrement d’un nouveau client
      clilen = sizeof(cli_addr);
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      printf("accept, socket: %i\n", newsockfd);
      if (newsockfd < 0) {
        perror("servmulti : erreur accept");
        exit(5);
      }

      // Recherche d’une place libre dans le tableau
      int i = 0;
      while ((i < FDSET_SIZE_CLIENT) && (tab_clients[i] != -1)) i++;
      if (i == FDSET_SIZE_CLIENT) {
        fprintf(stderr, "Plus de place libre pour un nouveau client\n");
        exit(6);
      }
      printf("place libre: %i\n", i);

      // Ajout du nouveau client au tableau des clients
      tab_clients[i] = newsockfd;

      // Ajout du nouveau client dans rset
      FD_SET(newsockfd, &rset);

      // Positionner maxfdp1
      if (newsockfd >= maxfdp1) {
        maxfdp1 = newsockfd + 1;
        printf("maxfdp1 mis à jour: %i\n", maxfdp1);
      }
      nbfd--;
    }

    // Parcourir le tableau des clients connectés
    int i = 0;
    int sock_client;
    while ((nbfd > 0) && (i < FDSET_SIZE_CLIENT)) {
      sock_client = tab_clients[i];
      if ((sock_client >= 0) && (FD_ISSET (sock_client, &pset))) {
        FD_CLR(sock_client, &pset);
        // Le client a envoyé une donnée, la traiter
        if ((childpid = fork()) < 0) {
          perror("server: fork error");
          exit(7);
        } else if (childpid == 0) { // Fils
          printf("Forked %i, sock_client %i\n", getpid(), sock_client);
          close(sockfd);
          // Dispatch request renvoie le nombre de donnée lues.
          int dispatch_result = 1;
          sqlite3 *db = open_db();
          /* Si aucune donnée n’a été lue, c’est que le client veut fermer la
           * connexion (condition du while à 0) */
          while (dispatch_result != 0) {
            printf("%i: DISPATCH_REQUEST %i\n", getpid(), dispatch_result);
            dispatch_result = dispatch_request(newsockfd, db);
          }
          printf("%i: Fermeture de la connexion au client %i…\n", getpid(), i);
          close_db(db);
          close(sock_client);
          exit(0);
        }
        // Fermeture socket, désenregistrement du client
        printf("%i: Fermeture de la connexion au client %i…\n", getpid(), i);
        close(sock_client);
        tab_clients[i]=-1;
        FD_CLR(sock_client, &rset);
        printf("%i: Connexion au client %i fermée !\n", getpid(), i);
        printf("Fin fork : %i\n", getpid());
      }
      i++;
    }
    printf("%i: ----- fin while\n", getpid());
  }
}


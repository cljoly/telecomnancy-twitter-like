#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "treat_request.h"
#include "const.h"

int str_echo(int sockfd) {
  int nrcv;
  int nsnd;
  char msg[BUFSIZE];

  // Attendre  le message envoye par le client
  memset((char *)msg, 0, sizeof(msg));
  if ((nrcv = read(sockfd, msg, sizeof(msg) - 1)) < 0) {
    perror("servmulti : : readn error on socket");
    exit(1);
  }
  msg[nrcv] = '\0';
  printf("servmulti :message recu=%s du processus %d nrcv = %d \n", msg,
         getpid(), nrcv);

  if ((nsnd = write(sockfd, msg, nrcv)) < 0) {
    printf("servmulti : writen error on socket");
    exit(1);
  }
  printf("nsnd = %d \n", nsnd);
  return (nsnd);
}



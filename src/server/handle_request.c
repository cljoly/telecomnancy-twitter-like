#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"

int dispatch_request(int sockfd) {
  int nrcv;
  int nsnd;
  char msg[BUFSIZE];

  // Attendre le message envoyé par le client
  memset((char *)msg, 0, sizeof(msg));
  if ((nrcv = read(sockfd, msg, sizeof(msg) - 1)) < 0) {
    perror("dispatch: readn error on socket");
    exit(8);
  }
  msg[nrcv] = '\0';
  printf("%d: nrcv: %d\n", getpid(), nrcv);
  printf("==>(%d) %s \n", getpid(), msg);

  // Réécrire le message envoyé par le client
  if ((nsnd = write(sockfd, msg, nrcv)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  printf("nsnd = %d \n", nsnd);
  return nrcv;
}



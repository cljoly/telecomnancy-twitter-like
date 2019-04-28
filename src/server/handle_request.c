#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json.h>
#include <sqlite3.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"
#include "methods.h"
#include "db.h"

int dispatch_request(int sockfd, sqlite3 *db) {
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

  // XXX Code seulement pour tester la base de données, à enlever ensuite
  create_account("toto", "1234", db);
  create_account("tuto", "1234", db);
  create_account("tutu", "1234", db);
  create_account("tota", "1234", db);
  create_account("totu", "1234", db);
  create_account("tuta", "1234", db);
  create_account("tata", "1234", db);

  // Réécrire le message envoyé par le client
  if ((nsnd = write(sockfd, msg, nrcv)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  printf("nsnd = %d \n", nsnd);
  return nrcv;
}


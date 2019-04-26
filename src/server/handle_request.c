#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"

// TODO Déplacer dans un autre fichier
#include <sqlite3.h>
#include "db.h"
#include <string.h>
#include <assert.h>

/**
 *  Récupération du cookie depuis une requête select
 */
int cookie_callback(void *cookie, int argc, char **argv, char **colName) {
  int *c = (int *)cookie;
  if (strcmp(colName[0], "cookie") != 0 || argc != 1)
    printf("========== cookie_callback exécuté dans de mauvaises conditions");
  *c = atoi(argv[0]);
  return 0;
}

// TODO Mettre ça dans un autre fichier
/** Création d’un compte dans la base de données
 * @param jparam objet json contenant les paramètres de la méthode
 * @return 0 tout s’est bien passé, autre : code d’erreur de la spec
 */
int create_account(char *user, char *pass) {
  sqlite3 *db = open_db();
  char stmt[BUFSIZE];
  // Pas de vérification de l’unicité du cookie même si la base de donnée le
  // vérifie : la proba de collision est faible à cause de la
  // taille du nombre aléatoire fournit
  sprintf(stmt,
      "INSERT INTO user (name, password, cookie) VALUES ('%s', '%s', ABS(RANDOM() %% %i))",
      user, pass, MAXVALUEINT-1);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  int cookie = -1.;
  sprintf(stmt, "SELECT cookie FROM user WHERE name='%s'", user);
  exec_db(db, stmt, &cookie_callback, &cookie);
  printf("COOKIE from callback: %i\n", cookie);
  close_db(db);
  return 0;
}

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

  // XXX Code seulement pour tester la base de données, à enlever ensuite
  create_account("toto", "1234");
  create_account("tuto", "1234");
  create_account("tutu", "1234");
  create_account("tota", "1234");
  create_account("totu", "1234");
  create_account("tuta", "1234");
  create_account("tata", "1234");

  // Réécrire le message envoyé par le client
  if ((nsnd = write(sockfd, msg, nrcv)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  printf("nsnd = %d \n", nsnd);
  return nrcv;
}


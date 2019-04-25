#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handle_request.h"
#include "const.h"

// TODO Déplacer dans un autre fichier
#include <sqlite3.h>
#include "db.h"

// TODO Mettre ça dans un autre fichier
/** Création d’un compte dans la base de données
 * @param jparam objet json contenant les paramètres de la méthode
 * @return 0 tout s’est bien passé, autre : code d’erreur de la spec
 */
int create_account(char *user, char *pass) {
  sqlite3 *db = open_db();
  char stmt[BUFSIZE];
  // Pas de vérification de l’unicité du cookie même si la base de donnée le
  // vérifie : la proba de collision est extremement faible à cause de la
  // taille du nombre aléatoire fournit, on a de meilleurs chances de gagner au
  // loto que de trouver une collision
  sprintf(stmt,
      "INSERT INTO user (name, password, cookie) VALUES ('%s', '%s', ABS(RANDOM()))",
      user, pass);
  exec_db(db, stmt);
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


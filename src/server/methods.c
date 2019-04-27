#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "db.h"
#include "json.h"
#include <string.h>
#include <assert.h>

#include "const.h"

/////// Méthode create_account ////////

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
int create_account(char *user, char *pass, sqlite3 *db) {
  char stmt[BUFSIZE];
  // Pas de vérification de l’unicité du cookie même si la base de donnée le
  // vérifie : la proba de collision est faible à cause de la
  // taille du nombre aléatoire fournit. On peut insérer 
  sprintf(stmt,
      "INSERT INTO user (name, password, cookie) VALUES ('%s', '%s', ABS(RANDOM() %% %i))",
      user, pass, MAXVALUEINT-1);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  int cookie = -1.;
  sprintf(stmt, "SELECT cookie FROM user WHERE name='%s'", user);
  exec_db(db, stmt, &cookie_callback, &cookie);
  printf("COOKIE from callback: %i\n", cookie);
  return 0;
}


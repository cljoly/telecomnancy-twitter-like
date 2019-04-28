#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <json.h>
#include <string.h>
#include <assert.h>

#include "methods.h"

#include "db.h"
#include "const.h"
#include "json_communication.h"

/**********************************************************************
*            Callback generiques pour les requêtes SELECT            *
**********************************************************************/

/**
 *  Renvoie le nombre de correspondances trouvées.
 *  Bien mettre nb_row à 0 avant d’appeler le callback
 */
int number_of_row_callback(void *nb_row, int argc, char **argv, char **colName) {
  int *n = (int *)nb_row;
  // Comme la fonction callback est appelée à chaque ligne, on compte le nombre de ligne en comptant le nombre d’appel
  printf("number_of_row_callback: nb_row: %i\n", *n);
  *n = *n+1;
  printf("=== number_of_row_callback: nb_row: %i\n", *n);
  // Affichage du contenu des lignes pour debuggage
	for(int i=0; i<argc; i++) {
		printf("%s = %s\n", colName[i], argv[i] ? argv[i] : "NULL");
	}
  return 0;
}


/**********************************************************************
*                       Méthode create_content                       *
**********************************************************************/

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

json_object *create_account(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  const char *user = json_object_get_string(
      json_object_object_get(params, "username"));
  const char *pass = json_object_get_string(
      json_object_object_get(params, "password"));

  char stmt[BUFSIZE];
  // Vérifions que le nom d’utilisateur soit libre
  sprintf(stmt, "SELECT * FROM user WHERE name='%s'", user);
  int nb_user = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_user);
  if (nb_user>0) {
    return create_answer(req, 1);
  }

  // Insérons le nom d’utilisateur
  /* Pas de vérification de l’unicité du cookie même si la base de donnée le
     vérifie : la proba de collision est faible à cause de la
     taille du nombre aléatoire fournit (jusqu’à une dizaine de clients). */
  sprintf(stmt,
      "INSERT INTO user (name, password, cookie)"\
      "VALUES ('%s', '%s', ABS(RANDOM() %% %i));",
      user, pass, MAXVALUEINT-1);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Récupérons le cookie
  int cookie = -1.;
  sprintf(stmt, "SELECT cookie FROM user WHERE name='%s'", user);
  exec_db(db, stmt, &cookie_callback, &cookie);
  printf("COOKIE from callback: %i\n", cookie);

  // Réponse
  json_object *answer = create_answer(req, 0);
  const char *answer_params[] = {
    "cookie",
    NULL
  };
  json_object *answer_values[] = {
    json_object_new_int(cookie),
    NULL
  };
  fill_answer(answer, answer_params, answer_values);
  return answer;
}

/**********************************************************************
*              Pour ce qui n’est pas encore implémenté               *
**********************************************************************/

json_object *not_implemented(json_object *req, sqlite3 *db) {
  printf("Not implemented, req: %s, db: %p",
      json_object_to_json_string(req), db);
  return create_answer(req, SPEC_ERR_NOT_IMPLEMENTED);
}

/**********************************************************************
*                        Gestion des méthodes                        *
**********************************************************************/

// TODO Compléter ça avec les autres méthodes de la spec
static char *method_names[] = {
  "create_account",
  NULL
};

static method_func_p method_funcs[] = {
  &create_account,
  &not_implemented
};

/**
 * Renvoie la méthode associée à un nom de méthode
 * @param meth_name Nom de la  méthode
 */
method_func_p find_associate_method_func(const char *meth_name) {
  int i = 0;
  while (method_names[i] != NULL
      && (strcmp(method_names[i], meth_name) != 0)) {
    i++;
  }
  printf("method_names[i]: i: %i\n", i);
  return method_funcs[i];
}

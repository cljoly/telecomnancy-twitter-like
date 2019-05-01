#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <json-c/json.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "methods.h"

#include "db.h"
#include "const.h"
#include "json_communication.h"

/**********************************************************************
*                  Callback pour les requêtes SELECT                 *
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

/**
 *  Récupération du nom d’utilisateur dans une requête SELECT
 */
int username_callback(void *username, int argc, char **argv, char **colName) {
  char *name = (char *)username;
  if (strcmp(colName[0], "name") != 0 || argc != 1)
    printf("========== username_callback exécuté dans de mauvaises conditions");
  strcpy(name, argv[0]);
  return 0;
}


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

// *jarray doit être un objet JSON de type tableau
int fill_users_array_callback(void *jarray, int argc, char **argv, char **colName) {
  json_object *ar = (json_object *)jarray;
  if ((strcmp(colName[0], "followed") != 0
        && strcmp(colName[0], "follower") != 0)
      || argc != 1)
    printf("========== fill_users_array_callback exécuté dans de mauvaises conditions");
  json_object_array_add(ar, json_object_new_string(argv[0]));
  return 0;
}

// *jarray doit être un objet JSON de type tableau
int fill_tags_array_callback(void *jarray, int argc, char **argv, char **colName) {
  json_object *ar = (json_object *)jarray;
  if (strcmp(colName[0], "tags") != 0 || argc != 1)
    printf("========== fill_tags_array_callback exécuté dans de mauvaises conditions");
  json_object_array_add(ar, json_object_new_string(argv[0]));
  return 0;
}

// *jarray doit être un objet JSON de type tableau
// Remplie le tableau de string contenant les identifiants (qui sont des nombres…)
int fill_ids_array_callback(void *jarray, int argc, char **argv, char **colName) {
  json_object *ar = (json_object *)jarray;
  if (strcmp(colName[0], "id") != 0 || argc != 1)
    printf("========== fill_ids_array_callback exécuté dans de mauvaises conditions");
  json_object_array_add(ar, json_object_new_string(argv[0]));
  return 0;
}

/**********************************************************************
*                    Autres fonctions génériques                     *
**********************************************************************/

void new_random_cookie(sqlite3 *db, const char *user) {
  char stmt[BUFSIZE];
  sprintf(stmt,
      "UPDATE user SET cookie=ABS(RANDOM() %% %i) "\
      "WHERE name='%s';",
      INT_MAX-1, user);
  exec_db(db, stmt, NULL, NULL);
}

/*
 * Récupère le nom d’utilisateur associé au cookie
 * @param user Est remplacé par le nom d’utilisateur trouvé, doit être de
 * longueur USERNAME_MAXSIZE
 * @return 1 si le cookie n’est pas trouvé (cookie invalide).
 */
int user_name_from_cookie(sqlite3 *db, int cookie, char *username) {
  char retrieved_username[USERNAME_MAXSIZE] = { '\0' };
  char stmt[BUFSIZE];
  sprintf(stmt, "SELECT name FROM user WHERE cookie='%i' LIMIT 1;",
      cookie);
  exec_db(db, stmt, &username_callback, &retrieved_username);
  printf("username récupéré : %s\n", retrieved_username);
  if (retrieved_username[0] == '\0') {
    printf("Pas d’username récupéré\n");
    return 1;
  }
  strncpy(username, retrieved_username, USERNAME_MAXSIZE);
  printf("username : %s\n", username);
  return 0;
}

// Ajout d’un gazouilli à la base de donnée
void new_gazou(sqlite3 *db, const char *gazou_content, const char *author,
    struct array_list *list_of_tags, const char *date) {
  // Ajout des thématiques
  char stmt[BUFSIZE];
  for (int i = 0; i < (int)array_list_length(list_of_tags); i++) {
    json_object *item = array_list_get_idx(list_of_tags, i);
    const char *tag_name = json_object_get_string(item);
    printf("list_of_tags[%i]: %s\n", i, tag_name);
    sprintf(stmt,
      "INSERT OR IGNORE INTO tag(name) VALUES('%s');",
      tag_name);
    exec_db(db, stmt, NULL, NULL);
  }

  // Ajouter le gazouilli
  sprintf(stmt,
      "INSERT INTO gazou(content, date, author) VALUES('%s', '%s', '%s');",
      gazou_content, date, author);
  exec_db(db, stmt, NULL, NULL);
  int gazou_id = sqlite3_last_insert_rowid(db);

  // Le lier aux thématiques
  for (int i = 0; i < (int)array_list_length(list_of_tags); i++) {
    json_object *item = array_list_get_idx(list_of_tags, i);
    const char *tag_name = json_object_get_string(item);
    sprintf(stmt,
      "INSERT OR IGNORE INTO gazou_tag(gazou_id, tag) VALUES(%i, '%s');",
      gazou_id, tag_name);
    exec_db(db, stmt, NULL, NULL);
  }
}

// Remplissage d’un tableau json contenant des ids de gazouilli par des objets gazouillis à partir de la base de donnée
void fill_gazou_array_from_string_id_array(sqlite3 *db, json_object *array_ids) {
  // TODO Implement this
  printf("%p %p\n", db, array_ids);
  /*
  // Ajout des thématiques
  char stmt[BUFSIZE];
  for (int i = 0; i < (int)array_list_length(list_of_tags); i++) {
    json_object *item = array_list_get_idx(list_of_tags, i);
    const char *tag_name = json_object_get_string(item);
    printf("list_of_tags[%i]: %s\n", i, tag_name);
    sprintf(stmt,
      "INSERT OR IGNORE INTO tag(name) VALUES('%s');",
      tag_name);
    exec_db(db, stmt, NULL, NULL);
  }

  // Ajouter le gazouilli
  sprintf(stmt,
      "INSERT INTO gazou(content, date, author) VALUES('%s', '%s', '%s');",
      gazou_content, date, author);
  exec_db(db, stmt, NULL, NULL);
  int gazou_id = sqlite3_last_insert_rowid(db);

  // Le lier aux thématiques
  for (int i = 0; i < (int)array_list_length(list_of_tags); i++) {
    json_object *item = array_list_get_idx(list_of_tags, i);
    const char *tag_name = json_object_get_string(item);
    sprintf(stmt,
      "INSERT OR IGNORE INTO gazou_tag(gazou_id, tag) VALUES(%i, '%s');",
      gazou_id, tag_name);
    exec_db(db, stmt, NULL, NULL);
  }
  */
}

/**********************************************************************
 *                     Méthode create_account                         *
 **********************************************************************/

json_object *create_account(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  const char *user = json_object_get_string(
      json_object_object_get(params, "username"));
  const char *pass = json_object_get_string(
      json_object_object_get(params, "password"));

  char stmt[BUFSIZE];
  // Vérifions que le nom d’utilisateur soit libre
  sprintf(stmt, "SELECT * FROM user WHERE name='%s';", user);
  int nb_user = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_user);
  if (nb_user>0) {
    return create_answer(req, SPEC_ERR_DUPLICATE_USERNAME);
  }

  // Insérons le nom d’utilisateur
  /* Pas de vérification de l’unicité du cookie même si la base de donnée le
     vérifie : la proba de collision est faible à cause de la
     taille du nombre aléatoire fournit (jusqu’à une dizaine de clients). */
  sprintf(stmt,
      "INSERT INTO user (name, password, cookie) "\
      "VALUES ('%s', '%s', ABS(RANDOM() %% %i));",
      user, pass, INT_MAX-1);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Réponse
  return create_answer(req, 0);
}

/**********************************************************************
*                          Méthode connect                           *
**********************************************************************/

json_object *connect(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  const char *user = json_object_get_string(
      json_object_object_get(params, "username"));
  const char *pass = json_object_get_string(
      json_object_object_get(params, "password"));

  char stmt[BUFSIZE];
  // Vérifions que le nom d’utilisateur existe
  sprintf(stmt, "SELECT * FROM user WHERE name='%s';", user);
  int nb_user = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_user);
  if (nb_user != 1) {
    printf("Nom d’utilisateur incorrect\n");
    return create_answer(req, SPEC_ERR_UNKNOWN_USERNAME);
  }
  
  // Vérifions que le mot de passe soit correct
  sprintf(stmt, "SELECT * FROM user WHERE password='%s';", pass);
  int nb_pass = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_pass);
  if (nb_pass != 1) {
    printf("Mot de passe incorrecte\n");
    return create_answer(req, SPEC_ERR_INCORRECT_PASSWORD);
  }

  // Réinitialisation du cookie
  new_random_cookie(db, user);

  // Récupérons le cookie
  int cookie = -1;
  sprintf(stmt, "SELECT cookie FROM user WHERE name='%s';", user);
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
*                         Méthode disconnect                         *
**********************************************************************/

json_object *disconnect(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));

  // Vérifions que le cookie soit correct en récupérant l’utilisateur
  char username[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, username);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  // Mise à jour du cookie, pour déconnecter
  new_random_cookie(db, username);

  json_object *answer = create_answer(req, 0);
  return answer;
}

/**********************************************************************
*                         Méthode send_gazou                         *
**********************************************************************/


json_object *send_gazou(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));

  json_object *gazou = json_object_object_get(params, "gazouilli");
  const char *gazou_content = json_object_get_string(
      json_object_object_get(gazou, "content"));
  struct array_list *gazou_tags = json_object_get_array(
      json_object_object_get(gazou, "list_of_tags"));
  const char *gazou_date = json_object_get_string(
      json_object_object_get(gazou, "date"));

  // Récupération du nom utilisateur
  char author[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, author);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  // TODO Vérifier éventuellement SPEC_ERR_INCORRECT_CHAR_IN_GAZOU (quand on
  // saura comment le faire)

  // Vérification de la longueur du gazouilli
  if (((int)strnlen(gazou_content, SPEC_GAZOU_SIZE+1)) > SPEC_GAZOU_SIZE) {
    printf("Gazoulli reçu trop long\n");
    return create_answer(req, SPEC_ERR_MESSAGE_TOO_LONG);
  }

  // Enregistrement du gazouilli
  new_gazou(db, gazou_content, author, gazou_tags, gazou_date);

  json_object *answer = create_answer(req, 0);
  return answer;
}

/**********************************************************************
*                        Méthode follow_user                         *
**********************************************************************/

json_object *follow_user(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));
  const char *username_to_follow = json_object_get_string(
      json_object_object_get(params, "username"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Vérifions que le nom d’utilisateur à suivre existe
  sprintf(stmt, "SELECT * FROM user WHERE name='%s';", username_to_follow);
  int nb_user = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_user);
  if (nb_user<1) {
    printf("Utilisateur à suivre inconnu\n");
    return create_answer(req, SPEC_ERR_UNKNOWN_USERNAME_TO_FOLLOW);
  }

  // Vérifions qu’on ne soit pas déjà abonné
  sprintf(stmt,
      "SELECT * FROM user_subscription "\
      "WHERE followed='%s' AND follower='%s';",
      username_to_follow, user);
  int nb = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb);
  if (nb>0) {
    printf("Utilisateur %s déjà suivi\n", username_to_follow);
    return create_answer(req, SPEC_ERR_ALREADY_FOLLOWING_USERNAME);
  }

  // Insérons l’information de suivie
  sprintf(stmt,
      "INSERT INTO user_subscription(followed, follower) "\
      "VALUES ('%s', '%s');",
      username_to_follow, user);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Réponse
  return create_answer(req, 0);

  json_object *answer = create_answer(req, 0);
  return answer;
}

/**********************************************************************
*                         Méthode follow_tag                         *
**********************************************************************/


json_object *follow_tag(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));
  const char *tag_to_follow = json_object_get_string(
      json_object_object_get(params, "tag"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Vérifions qu’on ne soit pas déjà abonné au tag
  sprintf(stmt,
      "SELECT * FROM tag_subscription "\
      " WHERE tag='%s' AND follower='%s';",
      tag_to_follow, user);
  int nb = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb);
  if (nb>0) {
    printf("Tag %s déjà suivi\n", tag_to_follow);
    return create_answer(req, SPEC_ERR_ALREADY_FOLLOWING_TAG);
  }

  // Insérons l’information de suivie
  sprintf(stmt,
      "INSERT INTO tag_subscription(tag, follower) "\
      "VALUES ('%s', '%s');",
      tag_to_follow, user);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Réponse
  return create_answer(req, 0);

  json_object *answer = create_answer(req, 0);
  return answer;
}

/**********************************************************************
*                       Méthode unfollow_user                        *
**********************************************************************/

json_object *unfollow_user(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));
  const char *username_to_unfollow = json_object_get_string(
      json_object_object_get(params, "username"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Vérifions que le nom d’utilisateur à cesser de suivre existe
  sprintf(stmt, "SELECT * FROM user WHERE name='%s';", username_to_unfollow);
  int nb_user = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb_user);
  if (nb_user<1) {
    printf("Utilisateur à cesser de suivre inconnu\n");
    return create_answer(req, SPEC_ERR_UNKNOWN_USERNAME_TO_UNFOLLOW);
  }

  // Vérifions qu’on soit bien déjà abonné
  sprintf(stmt,
      "SELECT * FROM user_subscription "\
      " WHERE followed='%s' AND follower='%s';",
      username_to_unfollow, user);
  int nb = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb);
  if (nb == 0) {
    printf("Non abonné à l’utilisateur %s\n", username_to_unfollow);
    return create_answer(req, SPEC_ERR_ALREADY_UNFOLLOWING_USERNAME);
  }

  // Supprimons l’information de suivie
  sprintf(stmt,
      "DELETE FROM user_subscription WHERE followed='%s' AND follower='%s';",
      username_to_unfollow, user);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Réponse
  return create_answer(req, 0);

  json_object *answer = create_answer(req, 0);
  return answer;
}


/**********************************************************************
*                       Méthode unfollow_tag                         *
**********************************************************************/

json_object *unfollow_tag(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));
  const char *tag_to_unfollow = json_object_get_string(
      json_object_object_get(params, "tag"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Vérifions qu’on soit bien déjà abonné au tag
  sprintf(stmt,
      "SELECT * FROM tag_subscription "\
      " WHERE tag='%s' AND follower='%s';",
      tag_to_unfollow, user);
  int nb = 0;
  exec_db(db, stmt, &number_of_row_callback, &nb);
  if (nb == 0) {
    printf("Tag %s non suivi\n", tag_to_unfollow);
    return create_answer(req, SPEC_ERR_ALREADY_UNFOLLOWING_TAG);
  }

  // Supprimons l’information de suivie
  sprintf(stmt,
      "DELETE FROM tag_subscription "\
      "WHERE tag='%s' AND follower='%s';",
      tag_to_unfollow, user);
  exec_db(db, stmt, NULL, NULL);
  memset(stmt, '\0', BUFSIZE);

  // Réponse
  return create_answer(req, 0);

  json_object *answer = create_answer(req, 0);
  return answer;
}

/**********************************************************************
*                     Méthode list_followed_user                     *
**********************************************************************/

json_object *list_followed_users(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Récupérations des noms des utilisateurs suivis
  sprintf(stmt, "SELECT followed FROM user_subscription WHERE follower='%s';", user);
  json_object *followed_users = json_object_new_array();
  exec_db(db, stmt, &fill_users_array_callback, followed_users);

  // Réponse
  json_object *answer = create_answer(req, 0);
  const char *answer_params[] = {
    "list_of_users",
    NULL
  };
  json_object *answer_values[] = {
    followed_users,
    NULL
  };
  fill_answer(answer, answer_params, answer_values);
  return answer;
}

/**********************************************************************
*                     Méthode list_followed_tags                     *
**********************************************************************/

json_object *list_followed_tags(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Récupérations des thématiques suivies
  sprintf(stmt, "SELECT tag FROM tag_subscription WHERE follower='%s';", user);
  json_object *followed_tags = json_object_new_array();
  exec_db(db, stmt, &fill_tags_array_callback, followed_tags);

  // Réponse
  json_object *answer = create_answer(req, 0);
  const char *answer_params[] = {
    "list_of_tags",
    NULL
  };
  json_object *answer_values[] = {
    followed_tags,
    NULL
  };
  fill_answer(answer, answer_params, answer_values);
  return answer;
}

/**********************************************************************
*                     Méthode list_my_followers                      *
**********************************************************************/

json_object *list_my_followers(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Récupérations des abonnés
  sprintf(stmt, "SELECT follower FROM user_subscription WHERE followed='%s';", user);
  json_object *followed_users = json_object_new_array();
  exec_db(db, stmt, &fill_users_array_callback, followed_users);

  // Réponse
  json_object *answer = create_answer(req, 0);
  const char *answer_params[] = {
    "list_of_followers",
    NULL
  };
  json_object *answer_values[] = {
    followed_users,
    NULL
  };
  fill_answer(answer, answer_params, answer_values);
  return answer;
}

/**********************************************************************
*                         Méthode get_gazou                          *
**********************************************************************/

json_object *get_gazou(json_object *req, sqlite3 *db) {
  json_object *params = json_object_object_get(req, "params");
  int cookie = json_object_get_int(
      json_object_object_get(params, "cookie"));
  int max_nb_gazou = json_object_get_int(
      json_object_object_get(params, "nb_gazou"));

  // Récupération du nom utilisateur
  char user[USERNAME_MAXSIZE];
  int r = user_name_from_cookie(db, cookie, user);
  if (r) {
    printf("Cookie incorrect\n");
    return create_answer(req, SPEC_ERR_INCORRECT_COOKIE);
  }

  char stmt[BUFSIZE];
  // Récupérations des ids des gazouillis
  sprintf(stmt,
      "SELECT DISTINCT id FROM gazou "\
      "LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id "\
      "LEFT JOIN relay ON gazou.id = relay.gazou_id "\
      "WHERE author IN (SELECT followed FROM user_subscription WHERE follower = '%s') "\
      "OR tag IN (SELECT tag FROM tag_subscription WHERE follower = '%s') "\
      "OR retweeter IN (SELECT followed FROM user_subscription WHERE follower = '%s') "\
      "ORDER BY date "\
      "LIMIT %i;",
      user, user, user, max_nb_gazou);
  json_object *gazou_ids = json_object_new_array();
  exec_db(db, stmt, &fill_ids_array_callback, gazou_ids);

  // Remplacement des id sous forme de string par des objets gazouillis
  fill_gazou_array_from_string_id_array(db, gazou_ids);

  // Réponse
  json_object *answer = create_answer(req, 0);
  const char *answer_params[] = {
    "list_of_gazous",
    NULL
  };
  json_object *answer_values[] = {
    gazou_ids,
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
  "connect",
  "disconnect",
  "send_gazou",
  "follow_user",
  "follow_tag",
  "unfollow_user",
  "unfollow_tag",
  "list_followed_users",
  "list_followed_tags",
  "list_my_followers",
  "get_gazou",
  NULL
};

static method_func_p method_funcs[] = {
  &create_account,
  &connect,
  &disconnect,
  &send_gazou,
  &follow_user,
  &follow_tag,
  &unfollow_user,
  &unfollow_tag,
  &list_followed_users,
  &list_followed_tags,
  &list_my_followers,
  &get_gazou,
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

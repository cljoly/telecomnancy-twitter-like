#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <json.h>
#include <sqlite3.h>

#include "handle_request.h"
#include "const.h"
#include "methods.h"
#include "db.h"
#include "json_communication.h"

// Attribut effectivement la requête à la fonction adéquate
// Renvoie la réponse qui sera faîte au client
json_object *actual_dispatch(char *msg, sqlite3 *db) {
  // Parser le message JSON envoyé par le client
  json_object *req = get_request_object(msg);
  json_object *a = NULL;
  printf("%d: JSON récupéré : %s \n", getpid(),
      json_object_to_json_string(req));
  json_object *method_name_obj = json_object_object_get(req, "request");
  const char *method_name = json_object_get_string(method_name_obj);
  if (req == NULL || method_name_obj == NULL) {
    fprintf(stderr, "Erreur en parsant le JSON reçu\n");
    a = create_answer(req, SPEC_ERR_FORMAT);
    return a;
  }

  printf("method_name %s\n", method_name);
  method_func_p mf = find_associate_method_func(method_name);
  a = (*mf)(req, db);

  // Libérer le json alloué
  json_object_put(req);

  if (a == NULL) {
    fprintf(stderr, "Erreur interne (a == NULL)\n");
    a = create_answer(req, SPEC_ERR_INTERNAL_SRV);
  }
  return a;
}

int dispatch_request(int sockfd, sqlite3 *db) {

  int nrcv;
  int nsnd;
  char msg[BUFSIZE] = {'\0'};

  // Attendre le message envoyé par le client
  memset((char *)msg, 0, sizeof(msg));
  if ((nrcv = read(sockfd, msg, sizeof(msg) - 1)) < 0) {
    perror("dispatch: readn error on socket");
    exit(8);
  }
  msg[nrcv] = '\0';
  printf("%d: nrcv: %d\n", getpid(), nrcv);
  printf("==>(%d) %s \n", getpid(), msg);

  // Traiter le message envoyé par le client
  json_object *answer = actual_dispatch(msg, db);
  if (answer == NULL) {
    // Ne devrait pas arriver, ce genre de cas est traité plus haut
    // normalement, renvoyer le code d’erreur interne
    fprintf(stderr, "ATTENTION, la réponse est NULL\n");
  }

  // Répondre au client
  const char *answer_string = json_object_to_json_string(answer);
  int answer_string_len = strnlen(answer_string, BUFSIZE);
  printf("<==(%d) %s\n", getpid(), answer_string);
  if ((nsnd = write(sockfd, answer_string, answer_string_len)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  json_object_put(answer);
  if (nsnd != answer_string_len) {
    fprintf(stderr, "ATTENTION : nsnd != answer_string_len, tout n’a pas été envoyé au client\n");
  }
  if ((nsnd = write(sockfd, "\n", 1)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  return nrcv;
}


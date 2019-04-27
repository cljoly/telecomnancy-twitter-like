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
json_object *actual_dispatch(char *msg) {
  // Parser le message JSON envoyé par le client
  json_object *req = get_request_object(msg);
  json_object *a = NULL;
  printf("%d: JSON récupéré : %s \n", getpid(),
      json_object_to_json_string(req));
  if (req == NULL) {
    fprintf(stderr, "Erreur en parsant le JSON reçu\n");
    a = create_answer(req, SPEC_ERR_FORMAT);
    return a;
  }
  a = create_answer(req, 0);
  // TODO Appel méthode,
  // Renvoyer retour de la méthode ou erreur interne si NULL
  // libérer le json alloué

  json_object_put(req);
  if (a == NULL) {
    fprintf(stderr, "Erreur interne (a == NULL)\n");
    a = create_answer(req, SPEC_ERR_INTERNAL_SRV);
  }
  return a;
}

int dispatch_request(int sockfd, sqlite3 *db) {
  // TODO Supprimer ce printf
  printf("db: %p", db);

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
  json_object *answer = actual_dispatch(msg);
  if (answer == NULL) {
    // Ne devrait pas arriver, ce genre de cas est traité plus haut
    // normalement, renvoyer le code d’erreur interne
    fprintf(stderr, "ATTENTION, la réponse est NULL\n");
  }

  // Répondre au client
  const char * answer_string = json_object_to_json_string(answer);
  printf("<==(%d) %s\n", getpid(), answer_string);
  if ((nsnd = write(sockfd, answer_string, nrcv)) < 0) {
    perror("servmulti : writen error on socket");
    exit(9);
  }
  json_object_put(answer);
  printf("nsnd = %d \n", nsnd);
  return nrcv;
}


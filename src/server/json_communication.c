#include <stdio.h>
#include <json.h>

#include "const.h"

/**
 * Fonction générique préparant un object JSON de réponse, avec l’identifiant de la requête
 * Cette fonction ne gère pas les paramètres
 * @param request Requête pour  laquelle on crée la réponse
 * @param error_code Mettre à 0 en cas de réponse pour une méthode qui s’est bien passée, sinon contien le code d’erreur
 * @return L'objet JSON pré-rempli
 */
json_object *create_answer(json_object* request, int error_code) {
    json_object *answer = json_object_new_object();
    if (answer == NULL) {
      return NULL;
    }
    json_object *method_name_obj = json_object_object_get(request, "request");
    json_object *id_obj = json_object_object_get(request, "id");
    if (request == NULL || method_name_obj == NULL || id_obj == NULL) {
      fprintf(stderr, "create_answer: %i, %i, %i\n", request == NULL,
          method_name_obj == NULL, id_obj == NULL);
      json_object_object_add(answer, "error",
          json_object_new_string(""));
      json_object_object_add(answer, "id", json_object_new_int(-1));
      json_object_object_add(answer, "error_code",
          json_object_new_int(SPEC_ERR_FORMAT));
      return answer;
    }
    const char *method_name = json_object_get_string(method_name_obj);
    int id = json_object_get_int(id_obj);
    if (error_code) { // Réponse avec erreur
      printf("Réponse avec erreur\n");
      json_object_object_add(answer, "error",
          json_object_new_string(method_name));
      json_object_object_add(answer, "id", json_object_new_int(id));
      json_object_object_add(answer, "error_code",
          json_object_new_int(error_code));
    } else { // Réponse sans erreur
      printf("Réponse sans erreur\n");
      json_object_object_add(answer, "result",
          json_object_new_string(method_name));
      json_object_object_add(answer, "id", json_object_new_int(id));
    }
    return answer;
}

/**
 * L’objet JSON fournit est-il une réponse ?
 * @return Renvoie 0 si l’objet est bien une réponse, 1 si c’est en fait une erreur et 2 si c’est autre chose
 */
int is_not_answer(json_object *o) {
    // TODO Implement this
    printf("Not implemented: is_not_answer (%s)\n",
        json_object_to_json_string(o));
    return 0;
}

/**
 * Compléter les params de la réponse
 * @param params_name Tableau de nom des paramètres, terminé par NULL
 * @param parmas_value Tableau d’objet JSON correspondant aux valeurs, terminé par NULL. Doit être de même longueure que le tableau des paramètres
 * @return 1 si l’objet answer n’est pas lié à une réponse sans erreur, 0 en l’absence d’erreur, autre code pour les autres erreurs
 */
int fill_answer(json_object* answer, const char** params_name, json_object *params_value[]) {
  if (is_not_answer(answer)) {
    return 1;
  }
  json_object* params = json_object_new_object();
  for (int i = 0; params_name[i] != NULL; i++) {
    if (params_value[i] == NULL) {
      return 2;
    }
    if (json_object_object_add(params, params_name[i], params_value[i]) != 0) {
      return 3;
    }
  }
  if (json_object_object_add(answer, "params", params) != 0) {
    return 4;
  }
  return 0;
}

/**
 * Récupère et lit la requête du serveur.
 * @param json_string Texte à essayer de parser en JSON
 * @return NULL en cas d'erreur, la réponse sinon.
 */
json_object* get_request_object(char *json_string) {
    json_object *response = json_tokener_parse(json_string);
    return response;
}


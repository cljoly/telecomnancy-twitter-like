#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "const.h"

#include "db.h"

static int empty_callback(void *NotUsed, int argc, char **argv, char **azColName){
  printf("NotUsed: %p\n", NotUsed);
	for(int i=0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

/**
 * Ouvre la base de données, pour pouvoir ensuite faire des requêtes
 * @return l’objet base de données crée. NULL en cas d’erreur
 */
sqlite3 *open_db(){
	sqlite3 *db;

	int rc = sqlite3_open(DB_NAME, &db);
	if(rc) {
		fprintf(stderr, "Impossible d’ouvrir la base de données: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(NULL);
	}
	return db;
}

/**
 * Gère la fermeture de la base de données, avec affichage des éventuelles erreurs
 */
void close_db(sqlite3 *db) {
	printf("Fermeture de la base de données\n");
	sqlite3_close(db);
}

/**
 * Exécute une requête SQL, affiche les messages d’erreurs si nécessaire
 * @param db Base de données ouverte avec open_db
 * @param statement Requête SQL
 * @param callback Callback pour exploiter notamment les select, utilisation
 * d’une fonction par défaut si cet argument est NULL
 * @param value Pointeur passé au callback
 * @return 0 si tout s’est bien passé, 1 si erreur
 */
int exec_db(sqlite3 *db, const char *statement, int (*callback)(void*,int,char**,char**), void *value) {
	printf("%i: Exécution de la requête '%s'\n", getpid(), statement);
	char *zErrMsg = 0;
  int (*cb)(void*,int,char**,char**) = empty_callback;
  if (callback != NULL) {
    cb = callback;
  }
	int rc = sqlite3_exec(db, statement, cb, value, &zErrMsg);
	printf("%i: Fin de l’exécution de la requête '%s'\n", getpid(), statement);
	if( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
    return 1;
	}
	return 0;
}

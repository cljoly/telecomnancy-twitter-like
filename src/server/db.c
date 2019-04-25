#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "const.h"

#include <db.h>

static int empty_callback(void *NotUsed, int argc, char **argv, char **azColName){
  printf("NotUsed: %p", NotUsed);
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
 * @return 0 si tout s’est bien passé, 1 si erreur
 */
int exec_db(sqlite3 *db, const char *statement) {
	printf("Va exécuter la requête %s\n", statement);
	char *zErrMsg = 0;
	// TODO Utiliser le callback avec un argument utilisateur ?
	int rc = sqlite3_exec(db, statement, empty_callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
    return 1;
	}
	return 0;
}

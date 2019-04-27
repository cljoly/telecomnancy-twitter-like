/* Treat request received from client */
#ifndef TREAT_REQUEST_H_KWMRYRLJ
#define TREAT_REQUEST_H_KWMRYRLJ
#include <sqlite3.h>

// Répartit les requêtes, renvoie le nombre d’octet lus
int dispatch_request(int sockfd, sqlite3 *db);


#endif /* end of include guard: TREAT_REQUEST_H_KWMRYRLJ */

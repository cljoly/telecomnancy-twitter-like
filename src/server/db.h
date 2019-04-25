#ifndef DB_H_GJIHP7XQ
#define DB_H_GJIHP7XQ
#include <sqlite3.h>

sqlite3 *open_db();
int exec_db(sqlite3 *db, const char *statement);
void close_db(sqlite3 *db);

#endif /* end of include guard: DB_H_GJIHP7XQ */

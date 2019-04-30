#ifndef METHODS_H_NBAPO82S
#define METHODS_H_NBAPO82S

#include <sqlite3.h>
#include <json-c/json.h>

// Type générique des fonctions qui gèrent les méthodes
typedef json_object *(method_func)(json_object *request, sqlite3 *db);
typedef method_func* method_func_p;

method_func_p find_associate_method_func(const char *meth_name);

#endif /* end of include guard: METHODS_H_NBAPO82S */

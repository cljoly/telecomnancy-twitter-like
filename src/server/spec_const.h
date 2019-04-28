#ifndef SPEC_CONST_H_DSA5DIFO
#define SPEC_CONST_H_DSA5DIFO

// Codes d’erreur de la spec
// commun
#define SPEC_ERR_FORMAT  10
#define SPEC_ERR_INTERNAL_SRV  11
#define SPEC_ERR_INTERNAL_CLIENT  12
#define SPEC_ERR_NOT_IMPLEMENTED  13
// create_account
#define SPEC_ERR_DUPLICATE_USERNAME  1
// connect
#define SPEC_ERR_UNKNOWN_USERNAME  1
#define SPEC_ERR_INCORRECT_PASSWORD  2
// send_gazou
#define SPEC_ERR_INCORRECT_CHAR_IN_GAZOU  1
#define SPEC_ERR_MESSAGE_TOO_LONG   2
// TODO Continuer avec la spec

#define DEFAULT_PORT 1234

#endif /* end of include guard: SPEC_CONST_H_DSA5DIFO */

#ifndef PARSER_H
#define PARSER_H

#include "montador_types.h"
#include "global.h"
#include <string.h>

int parseRead( program_t *program, token_t* c_tok);
int parseLoad( program_t *program, token_t* c_tok);
int parseStore( program_t *program, token_t* c_tok);
int parseBR( program_t *program, token_t* c_tok);
int parseBRNEG( program_t *program, token_t* c_tok);
int parseBRZERO( program_t *program, token_t* c_tok);
int parseBRPOS( program_t *program, token_t* c_tok);
int parseSUB( program_t *program, token_t* c_tok);
int parseADD( program_t *program, token_t* c_tok);
int parseWord( program_t *program, token_t* c_tok);
int parseIdentifier( program_t *program, token_t* c_tok);
int parseSection( program_t *program, token_t* c_tok);

#endif // PARSER_H

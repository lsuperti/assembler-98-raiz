#ifndef PARSER_H
#define PARSER_H

#include "montador_types.h"
#include "global.h"
#include <string.h>
#include "ligador.h"
#include "helper.h"

int parseAddressing( program_t *program, token_t* c_tok );
int parseRead( program_t *program, token_t* c_tok);
int parsePUT( program_t *program, token_t *c_tok ); 
int parseLoad( program_t *program, token_t* c_tok);
int parseDIVIDE( program_t *program, token_t* c_tok);
int parseMULT( program_t *program, token_t* c_tok);
int parseStore( program_t *program, token_t* c_tok);
int parseBR( program_t *program, token_t* c_tok);
int parseRET( program_t *program, token_t* c_tok);
int parseBRNEG( program_t *program, token_t* c_tok);
int parseBRZERO( program_t *program, token_t* c_tok);
int parseCALL( program_t *program, token_t* c_tok);
int parseBRPOS( program_t *program, token_t* c_tok);
int parseSUB( program_t *program, token_t* c_tok);
int parseADD( program_t *program, token_t* c_tok);
int parseWord( program_t *program, token_t* c_tok);
int parseAsciiz( program_t *program, token_t* c_tok);
int parseGLOBAL( program_t *program, token_t* c_tok);
int parseEXTERN( program_t *program, token_t* c_tok);
int parseIdentifier( program_t *program, token_t* c_tok);
int parseSection( program_t *program, token_t* c_tok);

#endif // PARSER_H

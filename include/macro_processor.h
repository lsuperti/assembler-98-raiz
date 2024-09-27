#ifndef MACRO_PROCESSOR_H
#define MACRO_PROCESSOR_H

#include "montador_types.h"
#include "helper.h"
#include "uthash.h"

#define error_rv int
#define _EINVAL -1*(EINVAL)
#define _ENOMEM -1*(ENOMEM)

void process_macros( program_t *program );
void replace_macro( program_t *program, MACRO_T *m1, MACRO_T *m2 );
void add_macro( program_t *program, MACRO_T *m );
MACRO_T *find_macro( program_t *program, char *name );
error_rv printMacros( program_t *p );
int compute_nargs( program_t *p );
void shiftTokens_reverse_by_amnt( token_t *tokens, size_t last_tok,
        int tok_idx, int amnt );
error_rv expandMode( 
  token_t **tokens, size_t *n_tokens, int tok_idx, MACRO_T *m, token_t *p_values,
  program_t *program );
void resetIdentifiers_Macros( program_t *p ); 

#endif

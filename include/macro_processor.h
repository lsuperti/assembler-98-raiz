#ifndef MACRO_PROCESSOR_H
#define MACRO_PROCESSOR_H

#include "montador_types.h"
#define error int
#define _EINVAL -22

void process_macros( program_t *program );
void add_macro( program_t *program, MACRO_T *m );
MACRO_T *find_macro( program_t *program, char *name );
error printMacros( program_t *p );
int compute_nargs( program_t *p );
void shiftTokens_reverse_by_amnt( token_t *tokens, size_t last_tok,
        int tok_idx, int amnt );

#endif

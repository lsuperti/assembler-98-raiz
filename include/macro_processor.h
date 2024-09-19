#ifndef MACRO_PROCESSOR_H
#define MACRO_PROCESSOR_H

#include "montador_types.h"

#define error_rv int
#define _EINVAL -1*(EINVAL)
#define _ENOMEM -1*(ENOMEM)

void process_macros( program_t *program );
void add_macro( program_t *program, MACRO_T *m );
MACRO_T *find_macro( program_t *program, char *name );
error_rv printMacros( program_t *p );
int compute_nargs( program_t *p );
void shiftTokens_reverse_by_amnt( token_t *tokens, size_t last_tok,
        int tok_idx, int amnt );

#endif

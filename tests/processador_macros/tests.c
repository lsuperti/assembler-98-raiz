
#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "montador.h"
#include "macro_processor.h"

Test( macro_processor_suite, test_SHIFT_AMNT )
{
    FILE *input = fopen("test_SHIFT_AMNT", "w");
    assert(input != NULL );
    char input_test[] = 
        " %macro SOME_MACRO PARAM1 PARAM2 \n"
        "   value: .word 50  \n";

    fprintf(input, "%s", input_test);
    fclose(input);
    input = fopen("test_SHIFT_AMNT", "r");
    program_t *program = createProgram(input);    
    fclose(input);
    tokenize(program);
    program->tokens = realloc( program->tokens,
         sizeof( token_t ) * ( program->n_tokens + 2 - ( 2 + 1 ) ) );

    shiftTokens_reverse_by_amnt( program->tokens, program->n_tokens - 1,
            1, 2 );
    printTokens(program);
}

Test( macro_processor_suite, test_SIMPLE_MACRO )
{
    FILE *input = fopen("test_simple_macro", "w");
    assert(input != NULL );
    char input_test[] = 
        " %macro SOME_MACRO PARAM1 PARAM2 \n"
        "    LOAD PARAM1 \n"
        "    LOAD PARAM2 \n"
        "    BRPOS loop  \n"
        " %macro SOME_MACRO PARAM1 PARAM2 \n"
        "    LOAD PARAM1 \n"
        "    LOAD PARAM2 \n"
        "    BRPOS loop  \n"
        "    STORE PARAM1 \n"
        " %endmacro      \n"
        "    STORE PARAM1\n"
        " %endmacro      \n"
        " %macro SUM_TWO ONE TWO \n" 
        "     LOAD ONE   \r\n"
        "     ADD  TWO   \n"
        " %endmacro      \n"
        " section .text  \n"
        "    loop:       \n"
        "     LOAD value \n"
        "     SOME_MACRO 20 50 \n"
        " section .data  \n"
        "   value: .word 50  \n";

    fprintf(input, "%s", input_test);
    fclose(input);
    input = fopen("test_simple_macro", "r");
    program_t *program = createProgram(input);    
    fclose(input);
    tokenize(program);
    process_macros(program);
    printMacros(program);
    fprintf(stdout, "\n\n");
    fflush(stdout);
    printTokens(program);
    
}

Test( macro_processor_suite, test_COMPUTE_NARGS )
{
    FILE *input = fopen("test_COMPUTE_NARGS", "w");
    assert(input != NULL );
    char input_test[] = 
        " macro_som PARAM1 PARAM2 30 &SOME_IDENTIFIER \n"
        " macro_som PARAM1 PARAM2 30 \n"
        " macro_som PARAM1 #30 \n"
        " macro_som  \n";
    fprintf(input, "%s", input_test);
    fclose(input);
    input = fopen("test_COMPUTE_NARGS", "r");
    program_t *program = createProgram(input);    
    fclose(input);
    tokenize(program);
    int n = compute_nargs(program);
    fprintf(stdout, "\n%d\n", n );
    fflush(stdout);
    program->token_idx+=7;
    n = compute_nargs(program);
    fprintf(stdout, "\n%d\n", n );
    fflush(stdout);
    program->token_idx+=5;
    n = compute_nargs(program);
    fprintf(stdout, "\n%d\n", n );
    fflush(stdout);
    program->token_idx+=5;
    n = compute_nargs(program);
    fprintf(stdout, "\n%d\n", n );
    fflush(stdout);
}

Test( macro_processor_suite, test_SIMPLE_EXPANSION ) 
{
}


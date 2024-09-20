
#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "montador.h"
#include "macro_processor.h"

//Test( macro_processor_suite, test_SHIFT_AMNT )
//{
//    FILE *input = fopen("test_SHIFT_AMNT", "w");
//    assert(input != NULL );
//    char input_test[] = 
//        " %macro SOME_MACRO PARAM1 PARAM2 \n"
//        "   value: .word 50  \n";
//
//    fprintf(input, "%s", input_test);
//    fclose(input);
//    input = fopen("test_SHIFT_AMNT", "r");
//    program_t *program = createProgram(input);    
//    fclose(input);
//    tokenize(program);
//    program->tokens = realloc( program->tokens,
//         sizeof( token_t ) * ( program->n_tokens + 2 - ( 2 + 1 ) ) );
//
//    shiftTokens_reverse_by_amnt( program->tokens, program->n_tokens - 1,
//            1, 2 );
//    printTokens(program);
//}
//
//Test( macro_processor_suite, test_SIMPLE_MACRO )
//{
//    FILE *input = fopen("test_simple_macro", "w");
//    assert(input != NULL );
//    char input_test[] = 
//        " %macro SOME_MACRO PARAM1 PARAM2 \n"
//        "    LOAD PARAM1 \n"
//        "    LOAD PARAM2 \n"
//        "    BRPOS loop  \n"
//        " %macro SOME_MACRO PARAM1 PARAM2 \n"
//        "    LOAD PARAM1 \n"
//        "    LOAD PARAM2 \n"
//        "    BRPOS loop  \n"
//        "    STORE PARAM1 \n"
//        " %endmacro      \n"
//        "    STORE PARAM1\n"
//        " %endmacro      \n"
//        " %macro SUM_TWO ONE TWO \n" 
//        "     LOAD ONE   \r\n"
//        "     ADD  TWO   \n"
//        " %endmacro      \n"
//        " section .text  \n"
//        "    loop:       \n"
//        "     LOAD value \n"
//        "     SOME_MACRO 20 50 \n"
//        " section .data  \n"
//        "   value: .word 50  \n";
//
//    fprintf(input, "%s", input_test);
//    fclose(input);
//    input = fopen("test_simple_macro", "r");
//    program_t *program = createProgram(input);    
//    fclose(input);
//    tokenize(program);
//    process_macros(program);
//    printMacros(program);
//    fprintf(stdout, "\n\n");
//    fflush(stdout);
//    printTokens(program);
//    
//}
//
//Test( macro_processor_suite, test_COMPUTE_NARGS )
//{
//    FILE *input = fopen("test_COMPUTE_NARGS", "w");
//    assert(input != NULL );
//    char input_test[] = 
//        " macro_som PARAM1 PARAM2 30 &SOME_IDENTIFIER \n"
//        " macro_som PARAM1 PARAM2 30 \n"
//        " macro_som PARAM1 #30 \n"
//        " macro_som  \n";
//    fprintf(input, "%s", input_test);
//    fclose(input);
//    input = fopen("test_COMPUTE_NARGS", "r");
//    program_t *program = createProgram(input);    
//    fclose(input);
//    tokenize(program);
//    int n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=7;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=5;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=5;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//}

//Test( macro_processor_suite, test_real ) 
//{
//    FILE *f = fopen("../../examples/highest_value_in_array.asm", "r" );
//    program_t *p = createProgram(f);
//    tokenize(p);
//    process_macros(p);
//    printMacros(p);
//    printTokens(p);
//}

Test( macro_processor_suite, test_SIMPLE_MACRO )
{
    FILE *input = fopen("test_simple_macro", "w");
    assert(input != NULL );
    char input_test[] = 
        " %macro EXIT CODE\n"
        "       PUT CODE \n"
        "       STOP  \n"
        " %endmacro\n"
        " %macro SOME_MACRO PARAM_H1 PARAM2 \n"
        "    LOAD PARAM_H1 \n"
        "    LOAD PARAM2 \n"
        "    BRPOS loop  \n"
        " %macro SOME_MACRO_COPY PARAM1 PARAM2 PARAM32 \n"
        "    LOAD PARAM1 \n"
        "    LOAD PARAM2 \n"
        "    SUB  PARAM_H1\n"
        "    BRPOS loop  \n"
        "    %%local_loop:\n"
        " %macro SOME_MACRO_COPY2 PARAM1 PARAM2 \n"
        "    LOAD  PARAM1 \n"
        "    STORE PARAM1 \n"
        " %macro THIRD ONE TWO\n"
        "        \n"
        " %endmacro\n"
        " %macro FIFTH ONE TWO\n"
        "       LOAD TWO\n"
        "       EXIT ONE \n"
        " %endmacro\n"
        "    STORE PARAM32 \n"
        " %endmacro      \n"
        "    BR %%local_loop\n"
        "    STORE PARAM1 \n"
        " %endmacro      \n"
        " %macro ANOTHER_ONE PARAM_LSODM \n"
        "   RET\n"
        " %endmacro\n"
        "    STORE PARAM_H1\n"
        " %endmacro      \n"
        " %macro SUM_TWO ONE TWO \n" 
        "     %%sum_twolor: \r\n"
        "     SOME_MACRO REALLY_COOL 50 \r\n"
        "     SOME_MACRO_COPY ONE 50 30 \r\n"
        "     SOME_MACRO FUCK_YOU 50 \r\n"
        "     SOME_MACRO_COPY ONE 50 30 \r\n"
        "     SOME_MACRO_COPY2 IDK 0x50 \r\n"
        "     LOAD ONE   \r\n"
        "     SOME_MACRO 0x300 5320\r\n"
        "     ADD  TWO   \n"
        "     CALL %%sum_twolor\n"
        " %endmacro      \n"
        " section .text  \n"
        "    loop:       \n"
        "     LOAD value \n"
        "     SUM_TWO value 0x320 \n"
        " section .data  \n"
        "   value: .word 50  \n"
        " %macro EXIT CODE\n"
        "       PUT CODE \n"
        "       RET \n"
        " %endmacro\n";

    fprintf(input, "%s", input_test);
    fclose(input);
    input = fopen("test_simple_macro", "r");
    program_t *program = createProgram(input);    
    fclose(input);
    tokenize(program);
    fprintf(stdout, "HELOL");
    fflush(stdout);
    process_macros(program);
    printMacros(program);
    fprintf(stdout, "\n\n");
    fflush(stdout);
    printTokens(program);
    parse(program);
    
}

//Test( macro_processor_suite, test_COMPUTE_NARGS )
//{
//    FILE *input = fopen("test_COMPUTE_NARGS", "w");
//    assert(input != NULL );
//    char input_test[] = 
//        " macro_som PARAM1 PARAM2 30 &SOME_IDENTIFIER \n"
//        " macro_som PARAM1 PARAM2 30 \n"
//        " macro_som PARAM1 #30 \n"
//        " macro_som  \n";
//    fprintf(input, "%s", input_test);
//    fclose(input);
//    input = fopen("test_COMPUTE_NARGS", "r");
//    program_t *program = createProgram(input);    
//    fclose(input);
//    tokenize(program);
//    int n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=7;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=5;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//    program->token_idx+=5;
//    n = compute_nargs(program);
//    fprintf(stdout, "\n%d\n", n );
//    fflush(stdout);
//}

Test( macro_processor_suite, test_SIMPLE_EXPANSION ) {}



#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "montador.h"
#include "macro_processor.h"

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



#include <criterion/criterion.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "montador.h"

Test(montador_suite, test_CREATE_PROGRAM) {

    FILE *input = fopen("test_create_program", "w");
    assert(input != NULL );
    char input_test[] = 
        " omasasoasdasdmo domsod \n"
        " ndsainasksndsjs !! !!!\n"
        " LOAD LOADL OAMDO SM \n";

    fprintf(input, "%s", input_test);
    fclose(input);
    input = fopen("test_create_program", "r");
    program_t *program = createProgram(input);    
    fclose(input);

    cr_assert_str_eq( input_test, program->source );
    cr_assert_eq( 0, program->HEAD );
    cr_assert_eq( NULL, program->sections );
    cr_assert_eq( NULL, program->table );
    cr_assert_eq( strlen(input_test), program->program_size );

    freeProgram(program);
}

Test(montador_suite, test_NEXT_TOKEN)
{

  //  FILE *input = fopen("test_next_token", "w");
  //  assert(input != NULL );
  //  char source[] = 
  //  " LOAD 20 STORE 29";

  //  fprintf(input, "%s", source);
  //  fclose(input);
  //  input = fopen("test_create_program", "r");
  //  program_t *program = createProgram(input);    
  //  fclose(input);

  //  token_t *tokens =
  //  (token_t *) malloc( 2 * sizeof ( token_t ) );
  //  tokens[0].token = "LOAD";
  //  tokens[1].token = "20";

  //  nextToken(program);

  //  cr_assert_str_eq( tokens[0].token, program->tokens[0].token ); 
    
}

Test(montador_suite, test_PARSE) {}

Test(montador_suite, test_TOKENIZE) {}

Test(montador_suite, test_APPEND_SECTIONS) {}

Test(montador_suite, test_INTERFACE) {}

//Test(montador_suite, test_ASSEMBLE_PROGRAM) {
//   
//    FILE *input = fopen("some_path", "r");
//    assert(input != NULL);
//    program_t *program = createProgram(input);
//    tokenize(program);
//    symbol_table_t *table = parse(program);
//    FILE *output = fopen("some_path_2", "w");
//    assert(output != NULL);
//    appendSections( program, output );   
//
//}


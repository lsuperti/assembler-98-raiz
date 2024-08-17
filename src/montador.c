
#include "montador.h"

// Expects that the file passed is in read mode,
// undefined behaviour otherwise.
section_t* getSection( FILE *file, const char *section_title )
{
    assert( file != NULL && section_title != NULL );
    assert( strcmp( section_title, ".text") == 0 || 
            strcmp( section_title, ".data") == 0 ||
            strcmp( section_title, ".rodata") == 0 );

    fseek(file, 0, SEEK_SET);

    section_t *section =
        (section_t *) malloc( sizeof (section_t) );
    assert(section != NULL);

    char line[MAX_LINE_LENGTH];
    assert( strlen(section_title) <= 41 ); 

    char *section_content = (char *) malloc( MAX_LINE_LENGTH );
    assert(section_content != NULL);
    section_content[0] = '\0';

    char section_seek[50] = "section ";
    strcat( section_seek, section_title );

    while( fgets(line, sizeof(line), file) ){
       if ( strstr(line, section_seek) != NULL )
           break;
    }
   
    while( fgets(line, sizeof(line), file ) ) {
        if ( strstr(line, "section") )
            break;

        char *temp =
        realloc( section_content, strlen(section_content) + strlen(line) + 1 );
        assert(section_content != NULL);
        section_content = temp;
        strcat( section_content, line );  
    }

    section->section_content = section_content;
    section->section_size    = strlen(section_content);
    section->section_title   = strdup(section_title);
    section->head            = 0;
    section->tokens          = NULL;

    if ( strcmp ( section_title, ".text" ) )
         section->section_type = EXECUTABLE;
    else if ( strcmp( section_title, ".data" ) )
         section->section_type = INITIALIZED_DATA;
    else 
         section->section_type = READONLY_DATA;

    return section;
}





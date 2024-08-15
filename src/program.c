
#include "program.h"
#include "architecture.h"
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include <gtk/gtk.h>

void manual_program() {

    FILE *f = fopen("../program", "r");
   
        if (!f) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        int in_text = false;
        char line[MAX_LINE_LENGTH];
        int idx = TEXT_SEGMENT_START;
        word_t num;
        while(fgets(line, sizeof(line), f)){

            if( (strstr(line, "section .text") != NULL) &&
                 !in_text ) {
                 in_text=true;
            }else if ( !in_text ) continue;
            
            if( strstr(line, "section .data") != NULL ) break;
            if( sscanf(line, "%hd", &num) ) {
                assert( &memory[idx] != NULL );
                if ( idx < DATA_SEGMENT_START && 
                     idx >= TEXT_SEGMENT_START ) {
                   memory[idx] = num; 
                }else {
                    g_print("Too many instructions");
                    fclose(f);
                    exit(EXIT_FAILURE);
                }
                idx++;
            }
        }

    fclose(f);

}



#include "program.h"
#include "architecture.h"
#include "global.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <gtk/gtk.h>

#define DEBUG_PRINT
#ifdef DEBUG_PRINT
    #define debug_print(fmt, ...) \
        do { fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#else
    #define debug_print(fmt, ...) \
        do { } while (0)
#endif

enum sections_t {
    NOT_IN_SECTION = 0,
    TEXT = 1,
    DATA = 2,
    RODATA = 3,
    number_of_sections
};

void load_program() {

    FILE *f = fopen(current_binary, "r");
   
        if (!f) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        int section = NOT_IN_SECTION; 

        char line[MAX_LINE_LENGTH];
        int idx = TEXT_SEGMENT_START;
        int data_reg_idx = DATA_SEGMENT_START;
        word_t inst_, data_, rodata_;

        fgets(line, sizeof(line), f); 
        if (strcmp(line, magic)) {
            g_print("File is not executable\n");
            exit(EXIT_FAILURE);
        }

        while(fgets(line, sizeof(line), f)){
            
            if ( strstr(line, "section .text") != NULL ) {
               section = TEXT; 
               goto SECTION_CHOOSER;
            }else if ( strstr(line, "section .data") != NULL ){
               section = DATA;
               goto SECTION_CHOOSER;
            }else if ( strstr(line, "section .rodata") != NULL ){
               section = RODATA;
               goto SECTION_CHOOSER;
            }else if ( section == NOT_IN_SECTION ){
                debug_print("NOT_IN_SECTION: %s", line);
                continue;
            }

SECTION_CHOOSER:
            switch(section) {

                case TEXT: 
                    debug_print("SECTION %d: %s",section, line);
                    while( fscanf(f, "%hd", &inst_) == 1 ) {
                        assert( idx < DATA_SEGMENT_START &&
                                idx >= TEXT_SEGMENT_START );

                        debug_print("SECTION %d: %d\n", section, inst_);

                        if ( idx < DATA_SEGMENT_START && 
                             idx >= TEXT_SEGMENT_START ) 
                             memory[idx] = inst_; 
                        else {
                            g_print("Too many instructions");
                            fclose(f);
                            exit(EXIT_FAILURE);
                        }
                        idx++;
                    }
                break;
                case DATA:
                    debug_print("SECTION %d: %s", section, line);
                    while( fscanf(f, "%hd", &data_) == 1 ) {

                        assert( data_reg_idx >= DATA_SEGMENT_START &&
                                data_reg_idx < MEMORY_SIZE );

                        debug_print("SECTION %d: %d\n", section, data_);

                        if ( data_reg_idx >= DATA_SEGMENT_START && 
                             data_reg_idx < MEMORY_SIZE ) 
                             memory[data_reg_idx] = data_; 
                        else {
                            g_print("Too much data");
                            fclose(f);
                            exit(EXIT_FAILURE);
                        }
                        data_reg_idx++;
                    }
                break;
                case RODATA:
                    debug_print("SECTION %d: %s", section, line);
                    while( fscanf(f, "%hx", &rodata_) == 1 ) {

                        assert( data_reg_idx >= DATA_SEGMENT_START &&
                                data_reg_idx < MEMORY_SIZE );

                        debug_print("SECTION %d: %d\n", section, rodata_);

                        if ( data_reg_idx >= DATA_SEGMENT_START && 
                             data_reg_idx < MEMORY_SIZE ) 
                             memory[data_reg_idx] = rodata_; 
                        else {
                            g_print("Too much data");
                            fclose(f);
                            exit(EXIT_FAILURE);
                        }
                        data_reg_idx++;
                    }
                break;
                default:
                    g_print("Unknown section %d", section);
                    fclose(f); 
                    exit(EXIT_FAILURE);
                break;

            }

        }

    fclose(f);

}


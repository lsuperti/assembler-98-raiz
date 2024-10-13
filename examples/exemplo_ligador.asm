
extern SHIFTL
extern SHIFTR
extern SHIFT_TIMES
extern FACTORIAL

global main
global end 

%macro UPDATE_MAX LABEL VALUE
    LOAD  VALUE    
    SUB   LABEL
    BRNEG %%bigger
    LOAD  VALUE
    STORE LABEL
%%bigger:
%endmacro

section .text
    main:
        UPDATE_MAX max 30
        LOAD 2
        STORE SHIFT_TIMES
        LOAD  max     
        CALL  SHIFTL    
        STORE max
        LOAD  6
        CALL  FACTORIAL
        ADD   max
        BR    end

        end:
        PUT  max
        STOP
section .data
    max: .space


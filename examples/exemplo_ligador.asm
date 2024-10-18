
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
        READ  value
        READ  f_times
        READ  SHIFT_TIMES
        UPDATE_MAX max value 
        LOAD  max     
        CALL  SHIFTL    
        STORE max
        LOAD  f_times 
        CALL  FACTORIAL
        ADD   max
        STORE max
        BR    end
    
    end:
        PUT  max
        STOP
section .data
    max:     .space
    value:   .space
    f_times: .space


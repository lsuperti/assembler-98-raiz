
global main
extern SHIFTL
extern SHIFTR

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
        UPDATE_MAX max_ 30
        CALL SHIFTL    
        STOP
section .data
    max_: .space


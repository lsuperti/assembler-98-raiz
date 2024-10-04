
global main
extern SHIFTL
global loop
extern SHIFTR
global max_

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
        loop:
        STOP
section .data
    max_: .space


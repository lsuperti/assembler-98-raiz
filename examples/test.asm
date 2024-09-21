global main

section .text

main:

    %macro SHIFTL RP
    STORE   TEMP3
    LOAD    RP
    STORE   TEMP4
    %%label:
    LOAD    TEMP3
    MULT    2
    STORE   TEMP3
    LOAD    TEMP4
    SUB     1
    STORE   TEMP4
    BRPOS   %%label
    LOAD    TEMP3
    %endmacro

    %macro SHIFTR RP
    STORE   TEMP3
    LOAD    RP
    STORE   TEMP4
    %%label:
    LOAD    TEMP3
    DIVIDE    2
    STORE   TEMP3
    LOAD    TEMP4
    SUB     1
    STORE   TEMP4
    BRPOS   %%label
    LOAD    TEMP3
    %endmacro

    %macro SCALE RP
    %macro NMULTSC  A B C
    LOAD    A
    MULT    B
    SHIFTR  RP
    STORE   C
    %endmacro
    %macro DIVSC A B C
    LOAD    A
    DIVIDE  B
    SHIFTL  RP
    STORE   C
    %endmacro
    %endmacro

    %macro DISCR A B THIRD D | 
    NMULTSC  A THIRD TEMP1
    NMULTSC  TEMP1 4 TEMP1
    NMULTSC  A B TEMP2
    SUB     TEMP1
    STORE   D
    %endmacro

    READ    A
    READ    B
    READ    C
    SCALE   3
    DISCR   A B C D
    PUT     D
    STOP

section .data
    A:      .space
    B:      .word 0
    C:      .space
    D:      .space
    TEMP1:  .space
    TEMP2:  .space
    TEMP3:  .space
    TEMP4:  .space

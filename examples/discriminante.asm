global main

section .text

main:

    *- 
       Shifts to the left 
       what is on the accumulator
       RP times, which equals
       multiplying by 2^( RP - 1 ) 
    -*
    %macro SHIFTL RP
    STORE   TEMP3
    LOAD    RP
    SUB     1
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

    *- 
       Shifts to the right
       what is on the accumulator
       RP times, which equals 
       dividing by 2^( RP ) -*
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
    %macro MULTSC  A B C
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

    %macro DISCR A B C D | 
    MULTSC  A C TEMP1
    MULTSC  TEMP1 4 TEMP1
    MULTSC  A B TEMP2
    SUB     TEMP1
    STORE   D
    %endmacro

    READ    A
    READ    B
    READ    THIRD
    SCALE   3

    DISCR   A B THIRD D
    PUT     D
    STOP

section .data
    A:      .space
    B:      .word 0
    THIRD:      .space
    D:      .space
    TEMP1:  .space
    TEMP2:  .space
    TEMP3:  .space
    TEMP4:  .space

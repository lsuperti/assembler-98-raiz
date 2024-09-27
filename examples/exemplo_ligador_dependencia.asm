
global SHIFTL
global SHIFTR

section .text
SHIFTL:
    STORE   TEMP3
    LOAD    SHIFT_VALUE
    SUB     1
    STORE   TEMP4
    shiftl_loop:
    LOAD    TEMP3
    MULT    2
    STORE   TEMP3
    LOAD    TEMP4
    SUB     1
    STORE   TEMP4
    BRPOS   shiftl_loop
    LOAD    TEMP3
    RET
SHIFTR:
    STORE   TEMP3
    LOAD    SHIFT_VALUE
    STORE   TEMP4
    shiftr_loop:
    LOAD    TEMP3
    DIVIDE    2
    STORE   TEMP3
    LOAD    TEMP4
    SUB     1
    STORE   TEMP4
    BRPOS   shiftr_loop
    LOAD    TEMP3
    RET

section .data
    TEMP3:       .space
    TEMP4:       .space
    SHIFT_VALUE: .space

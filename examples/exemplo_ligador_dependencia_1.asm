
global SHIFTL
global SHIFTR
global SHIFT_TIMES

-- Shifts left or right by shift_times amount.
-- the value that is in the accumulator at the moment of calling.

section .text
SHIFTL:
    STORE   TEMP3
    LOAD    SHIFT_TIMES
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
    LOAD    SHIFT_TIMES
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
    TEMP3: .space
    TEMP4: .space
    SHIFT_TIMES: .space

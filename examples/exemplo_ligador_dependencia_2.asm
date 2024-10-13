
global FACTORIAL

section .text
FACTORIAL:
        STORE TEMP1
        STORE PARTIAL

internal:
        LOAD  TEMP1    
        SUB   1
        STORE TEMP1
        MULT  PARTIAL

        STORE PARTIAL
        LOAD  TEMP1
        SUB   1
        BRPOS internal
        LOAD  PARTIAL
        RET
           
section .data
    TEMP1:   .space
    PARTIAL: .space

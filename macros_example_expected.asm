
global main

section .text

main:
    LOAD 20
    LOAD 20 
    RET
    STORE &value
    STORE 40
    LOAD 20
    RET
    STORE &30
    LOAD 20
    ADD 30
    RET 
    RET 
    ADD 30
    STORE value
    STORE 55
    STOP

section .data
    value: .word 20


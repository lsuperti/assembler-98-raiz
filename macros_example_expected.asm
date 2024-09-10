
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
    LOAD 50
    BRPOS label_example0000not_
    STOP
label_example0000not_:
    LOAD 75
    BRPOS label_example0001not_
    STOP
label_example0001not_:
    STOP

section .data
    value: .word 20


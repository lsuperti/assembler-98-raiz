
global main

section .text

main:
    READ N
    LOAD N
    BRNEG  zero
    BRZERO zero
    SUB 1
    BRZERO one
    LOAD N
    SUB 1
    STORE N
    
loop:
    LOAD  a
    ADD   b
    STORE FIB
    
    LOAD b
    STORE a

    LOAD FIB
    STORE b

    LOAD N
    SUB 1
    STORE N
    BRPOS loop
    STOP

zero:
    LOAD 0
    STORE FIB
    STOP

one:
    LOAD 1
    STORE FIB
    STOP
    
section   .data
    N:    .space
    FIB:  .space
    a:    .word 0
    b:    .word 1 
    TEMP: .space
    I:    .word 2


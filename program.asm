
global main

section .text

main:
    READ Number_of_times
    LOAD Number_of_times 
    BRNEG  zero
    BRZERO zero 
    SUB 1
    BRZERO one
    LOAD Number_of_times
    SUB 1
    STORE Number_of_times
    
loop:
    LOAD  a1
    ADD   b1
    STORE Fibonnac1
    
    LOAD b1
    STORE a1

    LOAD Fibonnac1
    STORE b1

    LOAD Number_of_times
    SUB 1
    STORE Number_of_times
    BRPOS loop
    PUT Fibonnac1
    STOP

zero:
    LOAD 0
    STORE Fibonnac1
    PUT Fibonnac1
    STOP

one:
    LOAD 1
    STORE Fibonnac1
    PUT Fibonnac1
    STOP
    
section   .data
    Number_of_times:    .space
    Fibonnac1:  .space
    a1:    .word 0
    b1:    .word 1 



global main

section .text
    main: 
    PUT value 
    LOAD value
    SUB 1 
    STORE value
    BRPOS main
    STOP

section .data
    value: .word 20

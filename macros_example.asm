
%macro assign 2
    LOAD %1
    RET
    STORE %2
%endmacro

%macro do_something 3
   STORE %3
   assign %1 %2
%endmacro

%macro do_anything 1
    %macro do_nothing 2
        RET
        RET
        ADD %1
        STORE %3
        STORE %2
    %endmacro
    LOAD 20
    ADD %1
%endmacro

%macro nothing 
%endmacro

global main

section .text

main:
    LOAD 20
    assign 20 &value
    do_something 20 &30 40
    do_anything 30
    do_nothing 55 value
    nothing
    STOP

section .data
    value: .word 20


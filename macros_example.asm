
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

*- 
Exemplo macro onde passado um argumento 
o programa encerra se tal for positivo. 
{%%label} local a macro.
-*
%macro label_example 1
    LOAD %1
    BRPOS %%not_
    STOP
  %%not_:
%endmacro

global main

section .text

main:
    LOAD 20
    assign 20 &value
    do_something 20 &30 &40
    do_anything 30
    do_nothing &55 value
    nothing
    label_example 50
    label_example 75
    STOP

section .data
    value: .word 20


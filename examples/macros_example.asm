
%macro assign ONE TWO
    LOAD ONE
    RET
    STORE TWO
%endmacro

%macro do_something MO NS TO
   STORE  TO
   assign MO NS
%endmacro

%macro do_anything    MON2
    %macro do_nothing MON XY
        RET
        RET
        ADD MON
        STORE XY
        ADD  MON2
        STORE MON
    %endmacro
    LOAD 20
    ADD  MON
%endmacro

%macro nothing 
%endmacro

*- 
Exemplo macro onde passado um argumento 
o programa encerra se tal for não positivo. 
{%%label} local a macro. -*

%macro label_example ONE
    LOAD ONE
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


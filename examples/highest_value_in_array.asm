
global main

%macro UPDATE_MAX label
    LOAD   label
    SUB    max_
    BRNEG  %%keep_max
    LOAD   label
    STORE  max_
%%keep_max:
%endmacro

%macro SUM ONE TWO
    LOAD ONE
    ADD TWO
%endmacro

section .text

    main:
    UPDATE_MAX value0 
    UPDATE_MAX value1
    UPDATE_MAX value2
    UPDATE_MAX value3
    UPDATE_MAX value4
    UPDATE_MAX value5
    UPDATE_MAX value6
    PUT max_
    STOP


section .data
    -- Array of values to find the maximum.
    num_elems: .word 7
    max_:      .space
    value0:    .word 20
    value1:    .word 30
    value2:    .word 32
    value3:    .word 1
    value4:    .word 50
    value5:    .word 320
    value6:    .word 43

    *-
       This code implements the straight foward way of finding 
       the maximum of an array of naturals which is to keep the maximum
       and iterate through all the items while updating the maximum
       of course this happens at O(n).
       We didn't implement any array structure to automatically allocate
       so every item is a word.
    -*

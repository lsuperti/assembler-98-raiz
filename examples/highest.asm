global main 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
section .text 
main: LOAD 
value0 SUB 
max_ BRNEG 
UPDATE_MAX_8245486993943867437_1_keep_max LOAD 
value0 STORE 
max_ UPDATE_MAX_8245486993943867437_1_keep_max: 
BLANK BLANK 
LOAD value1 
SUB max_ 
BRNEG UPDATE_MAX_8245486993943867437_2_keep_max 
LOAD value1 
STORE max_ 
UPDATE_MAX_8245486993943867437_2_keep_max: BLANK 
BLANK LOAD 
value2 SUB 
max_ BRNEG 
UPDATE_MAX_8245486993943867437_3_keep_max LOAD 
value2 STORE 
max_ UPDATE_MAX_8245486993943867437_3_keep_max: 
BLANK BLANK 
LOAD value3 
SUB max_ 
BRNEG UPDATE_MAX_8245486993943867437_4_keep_max 
LOAD value3 
STORE max_ 
UPDATE_MAX_8245486993943867437_4_keep_max: BLANK 
BLANK LOAD 
value4 SUB 
max_ BRNEG 
UPDATE_MAX_8245486993943867437_5_keep_max LOAD 
value4 STORE 
max_ UPDATE_MAX_8245486993943867437_5_keep_max: 
BLANK BLANK 
LOAD value5 
SUB max_ 
BRNEG UPDATE_MAX_8245486993943867437_6_keep_max 
LOAD value5 
STORE max_ 
UPDATE_MAX_8245486993943867437_6_keep_max: BLANK 
BLANK LOAD 
value6 SUB 
max_ BRNEG 
UPDATE_MAX_8245486993943867437_7_keep_max LOAD 
value6 STORE 
max_ UPDATE_MAX_8245486993943867437_7_keep_max: 
BLANK BLANK 
PUT max_ 
| LOAD 
20 ADD 
30 BLANK 
BLANK BLANK 
STORE summed 
LOAD 20 
MULT 30 
ADD 20 
BLANK BLANK 
BLANK BLANK 
BLANK BLANK 
BLANK STOP 
section .data 
num_elems: .word 
7 max_: 
.space summed: 
.space value0: 
.word 20 
value1: .word 
30 value2: 
.word 32 
value3: .word 
1 value4: 
.word 50 
value5: .word 
320 value6: 
.word 43 
EOF 
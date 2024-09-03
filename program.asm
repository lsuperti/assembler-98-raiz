
-- Exemplo de como seria a sintaxe do nosso montador
-- instruções no registrador DATA_REG não são possíveis
-- DATA_REG = CONST 767

-- Diferenças do trabalho para a nossa sintaxe : 
-- &  -> Endereçamento Direto
-- #  -> Endereçamento Indireto
-- "" -> Endereçamento Imediato

*- 
    COMENTARIO 
    BLOCO 
-* 

{ global, main, .extern, InitWindow, 
  STACK, 55, section, .text, main:, 
  READ, &, 788, READ, &, DATA_REG, ... }

global main        -- == INTDEF main 

.extern InitWindow -- == INTUSE InitWindow 
-- ( main e InitWindow apenas são endereços de memoria (labels) )
                    
STACK 55 -- Tem que ser um valor entre ( 0 - 255 ) Words
          
-- Número hexa : 0x230
-- Número decimal : 230

section .text  -- == START 

*-
  Esse é o mesmo codigo em program.elf98
  so que aqui em mnemônicos.
-* 

  main:
	READ   &788
	READ   &DATA_REG 
    READ   &(DATA_REG + 1)     
	LOAD   &(DATA_REG)
    ADD    &(DATA_REG + 1)
    ADD    #(DATA_REG / 4)
    STORE  &(DATA_REG + 2)
    WRITE  msg_helloworld
    STOP

section .data

    value1    .word  123    -- == CONST 123
    value2    .word  457    -- == CONST 457 
    value3    .word  232    -- == CONST 232 
    value4    .word  10024  -- == CONST 10024
    msg_helloworld .asciiz "Hello, World!"     








































#include "program.h"
#include "architecture.h"
#include <gtk/gtk.h>

void manual_program() {

    // Algoritmo simples que checa se
    // a soma de dois números lidos é
    // ZERO, POSITIVO OU NEGATIVO
    // o arquivo basic-algorithm
    // mostra os mnemônicos.

    // Injetado direto na memoria
    // por agora
    // depois criando o montador
    // é so dar um append na memoria.

    memory[256] = 12;
    memory[257] = data_reg;
    memory[258] = 12;
    memory[259] = data_reg + 1;
    memory[260] = 3;
    memory[261] = data_reg;
    memory[262] = 2;
    memory[263] = data_reg + 1;
    memory[264] = 7;
    memory[265] = data_reg + 2;
    memory[266] = 4;
    memory[267] = data_reg + 13;
    memory[268] = 1;
    memory[269] = data_reg + 14;
    memory[270] = 5;
    memory[271] = data_reg + 15;
    memory[272] = 11;
    
    memory[321] = 8;
    memory[322] = data_reg + 3;
    memory[323] = 11;

    memory[421] = 8;
    memory[422] = data_reg + 16;
    memory[423] = 11;

    memory[521] = 8;
    memory[522] = data_reg + 35;
    memory[523] = 14;
    memory[524] = data_reg + 70;
    memory[525] = 10;
    memory[526] = data_reg + 100;
    memory[527] = 20;
    memory[528] = data_reg + 2;

    memory[529] = 13;
    memory[530] = data_reg + 52;
    memory[531] = data_reg + 56;
    memory[532] = 00;
    memory[533] = data_reg + 45;
    memory[534] = 11;

    memory[600] = 15;
    memory[601] = data_reg + 200;
    memory[602] = 11;

    memory[455] = 12;
    memory[456] = data_reg + 221;
    memory[457] = 3;
    memory[458] = data_reg + 221;
    memory[459] = 16;

    memory[data_reg + 200] = 455;
    memory[data_reg + 52] = 50;
    memory[data_reg + 56] = 150;

    memory[data_reg + 13] = 321;
    memory[data_reg + 14] = 421;
    memory[data_reg + 15] = 521;
    memory[data_reg + 70] = 40;
    memory[data_reg + 100] = 50;
    memory[data_reg + 45] = 600;

    // Copies the strings to the data_reg buffer, including the null terminator
    memcpy(&memory[data_reg + 35], "negative", 9);
    memcpy(&memory[data_reg + 3], "zero", 5);
    memcpy(&memory[data_reg + 16], "positive", 9);

}


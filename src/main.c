/*HEADER***********************************************************************
 * INSTITUITION  : IFSul - Passo Fundo
 * COURSE/SUBJECT: BCC – Compilers – AT01: Lex Analysis Work
 * DATE          : 2024-04-28
 * FILENAME      : main.c
 * DESCRIPTION   :
 *     Scanner implementation for G2
 * AUTHOR(S)     : Luan Daros & Luiz Brugnera
 **************************************************************************END*/
#include <stdio.h>
#include <stdlib.h>

#include "G2-scanner-lib.h"

int main(int argc, char const *argv[])
{
    // Ler arquivo
    const char *filename = "G2-cadeias_entrada.txt";
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Could not open file %s\n", filename);
        return 1;
    }

    Token *token;
    int line = 1;   // Linha atual
    int column = 0; // Coluna atual
    while ((token = get_token(file, &line, &column)) != NULL)
    {
        printf("#%-2d:%-2d -> %-12s -> %s -> %s\n",
               token->line,
               token->column,
               token_type_str[token->type],
               token->type == TKN_REJECT ? "Reject" : "Accept",
               token->lexeme);

        free_token(token);
    }

    fclose(file);
    return 0;
}

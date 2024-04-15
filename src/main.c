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
    while ((token = getToken(file)) != NULL)
    {
        printf("#%d:%d -> %s -> %s -> %s\n",
               token->line,
               token->column,
               token_type_str[token->type],
               token->type == TKN_REJECT ? "Reject" : "Accept",
               token->lexeme);

        freeToken(token);
    }

    fclose(file);
    return 0;
}

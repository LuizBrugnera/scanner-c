#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G2-scanner-lib.h"

const char *token_type_str[] = {
    "TKN_COMMENT",
    "TKN_VAR",
    "TKN_RESWORD",
    "TKN_LTEQ",
    "TKN_GTEQ",
    "TKN_EQ",
    "TKN_DIFF",
    "TKN_ASSIGN",
    "TKN_NUMINT",
    "TKN_NUMHEX",
    "TKN_STMSEP",
    "TKN_LEFTPAR",
    "TKN_RIGHTPAR",
    "TKN_REJECT",
};

// Palavras reservadas
const char *reswords[] = {
    "if",
    "then",
    "else",
    "end",
    "repeat",
    "until",
    "read",
    "write",
};

// Função para alocar memória para um novo token
Token *createToken(enum TokenType type, int line, int col_start, const char *lexeme)
{
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->line = line;
    token->column = col_start;
    token->lexeme = strdup(lexeme);
    return token;
}

// Função para liberar a memória de um token
void freeToken(Token *token)
{
    free(token->lexeme);
    free(token);
}

char *delimeter_to_string(char c)
{
    char *str;

    switch (c)
    {
    case ' ':
        str = "SP"; // Space
        break;
    case '\t':
        str = "HT"; // Horizontal Tab
        break;
    case '\n':
        str = "LF"; // Line Feed
        break;
    case '\r':
        str = "CR"; // Carriage Return
        break;
    default:
        str = "UNK";
        break;
    }

    return str;
}

Token *getToken(FILE *file)
{
    static int line = 1;
    static int column = 0;
    char c;

    int buffer_size = 1000;
    char buffer[buffer_size]; // Usando um buffer de tamanho fixo
    int buf_index = 0;        // Índice para adicionar caracteres ao buffer

    while ((c = fgetc(file)) != EOF)
    {
        column++;

        // Delimitadores
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            int line_start = line;
            int col_start = column;

            if (c == '\n')
            {
                line++;
                column = 0; // Reinicia a contagem de colunas na nova linha
            }

            // Cria e retorna token de rejeição para delimitadores
            return createToken(TKN_REJECT, line_start, col_start, delimeter_to_string(c));
        }

        // Comentários
        if (c == '{')
        {
            int col_start = column;         // Início do comentário
            memset(buffer, 0, buffer_size); // Limpa o buffer
            buffer[buf_index++] = c;        // Adiciona '{' ao buffer

            // Lê o restante do comentário
            while ((c = fgetc(file)) != EOF && buf_index < buffer_size - 1)
            {
                buffer[buf_index++] = c == '\n' ? ' ' : c;
                column++;

                if (c == '}')
                {
                    buffer[buf_index] = '\0'; // Finaliza a string no buffer
                    return createToken(TKN_COMMENT, line, col_start, buffer);
                }
            }

            // Se sair do loop sem fechar o comentário
            if (c == EOF)
            {
                buffer[buf_index] = '\0';
                return createToken(TKN_REJECT, line, col_start, "Comentário não fechado");
            }
        }
    }

    // Se alcançar EOF sem encontrar mais tokens
    return NULL;
}

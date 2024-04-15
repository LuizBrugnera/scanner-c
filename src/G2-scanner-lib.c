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
Token *createToken(enum TokenType type, int line, int col_start, int col_end, const char *lexeme)
{
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->line = line;
    token->column_start = col_start;
    token->column_end = col_end;
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
        str = "Unknown";
        break;
    }

    return str;
}

// Função para obter o próximo token
Token *getToken(FILE *file)
{
    static int line = 1;
    static int column = 0;
    static char buffer[1024];
    static int buf_pos = 0;
    static int buf_len = 0;

    if (buf_pos >= buf_len)
    {
        if (fgets(buffer, sizeof(buffer), file) == NULL)
        {
            return NULL; // Fim do arquivo
        }
        buf_len = strlen(buffer);
        buf_pos = 0;
        column = 0;
    }

    while (buf_pos < buf_len)
    {
        char current = buffer[buf_pos];

        // Detectar comentários
        if (current == '{')
        {
            int start = column;
            buf_pos++;
            column++;
            while (buf_pos < buf_len && buffer[buf_pos] != '}')
            {
                buf_pos++;
                column++;
            }
            if (buf_pos < buf_len && buffer[buf_pos] == '}')
            {
                buf_pos++; // Passar por cima do '}'
                column++;
                return createToken(TKN_COMMENT, line, start, column, "Comentário");
            }
            else
            {
                // Final do buffer alcançado sem fechar o comentário
                // Aqui você poderia tratar isso como um comentário não fechado ou esperar por mais dados
                return createToken(TKN_REJECT, line, start, column, "Comentário não fechado");
            }
        }

        // Detectar caracteres rejeitados (espaço, tabulação, etc.)
        if (current == ' ' || current == '\t' || current == '\n' || current == '\r')
        {
            int start = column;
            buf_pos++;
            column++;
            return createToken(TKN_REJECT, line, start, column, delimeter_to_string(current));
        }

        buf_pos++;
        column++;
    }

    // Se chegamos ao final do buffer sem retornar, tentamos ler mais dados
    if (buf_pos >= buf_len)
    {
        buf_pos = 0; // Reset buffer position for the next read
        buf_len = 0; // Reset buffer length to force a read in the next call
    }

    return NULL;
}

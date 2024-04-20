/*HEADER***********************************************************************
 * INSTITUITION  : IFSul - Passo Fundo
 * COURSE/SUBJECT: BCC – Compilers – AT01: Lex Analysis Work
 * DATE          : 2024-04-28
 * FILENAME      : G2-scanner-lib.c
 * DESCRIPTION   :
 *     Scanner implementation for G2
 * AUTHOR(S)     : Luan Daros & Luiz Brugnera
 **************************************************************************END*/
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

const int reswords_size = sizeof(reswords) / sizeof(reswords[0]);

// Aloca memória para um novo token
Token *new_token(enum TokenType type, int line, int col_start, const char *lexeme)
{
    Token *token = malloc(sizeof(Token));
    token->type = type;
    token->line = line;
    token->column = col_start;
    token->lexeme = strdup(lexeme);
    return token;
}

// Libera a memória alocada para um token
void free_token(Token *token)
{
    free(token->lexeme);
    free(token);
}

// Converte um delimitador para uma string
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

// Atualiza a linha e a coluna
void new_line(int *line, int *column)
{
    (*line)++;
    *column = 0;
}

// Detecta delimitadores (space, tab, newline, carriage return)
int is_delimeter(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Detecta apenas letras
int is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Detecta apenas digitos
int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

// Detecta apenas digitos hexadecimais
int is_hex_digit(char c)
{
    return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

// Detecta apenas digitos inteiros
int is_int_digit(char c)
{
    return is_digit(c) || c == '-';
}

// Detecta início de operadores
int is_operator_start(char c)
{
    return c == ':' || c == '=' || c == '<' || c == '>' || c == '!';
}

// Detecta palavras reservadas
int is_resword(char *word)
{
    for (int i = 0; i < reswords_size; i++)
    {
        if (strcmp(word, reswords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

// Detecta caracteres de parada
int is_stop_char(char c)
{
    return c == EOF || is_delimeter(c) || is_operator_start(c) || c == ';' || c == '(' || c == ')';
}

// Função auxiliar para ler palavras
char *read_word(FILE *file, char initial, int *column)
{
    static char buffer[BLOCK_SIZE];
    int buf_index = 0;

    buffer[buf_index++] = initial;
    char c;

    // Continua a leitura ate encontrar um delimitador
    while ((c = fgetc(file)) != EOF && !is_stop_char(c))
    {
        buffer[buf_index++] = c;
        (*column)++;
    }

    // Se a leitura foi interrompida por um delimitador, devolve o caractere
    if (c != EOF)
    {
        ungetc(c, file);
    }

    buffer[buf_index] = '\0';

    return buffer;
}

// Processa delimitadores, separadores e parenteses
Token *process_delimiter_sep_par(FILE *file, char c, int *line, int *column)
{
    int line_start = *line;
    int col_start = *column;

    if (c == ';')
    {
        return new_token(TKN_STMSEP, line_start, col_start, ";");
    }

    if (c == '(')
    {
        return new_token(TKN_LEFTPAR, *line, *column, "(");
    }

    if (c == ')')
    {
        return new_token(TKN_RIGHTPAR, *line, *column, ")");
    }

    // Atualiza a linha e a coluna
    if (c == '\n')
    {
        new_line(line, column);
    }

    return new_token(TKN_REJECT, line_start, col_start, delimeter_to_string(c));
}

// Processa comentários
Token *process_comment(FILE *file, int *line, int *column)
{
    char buffer[BLOCK_SIZE] = {0};
    int buf_index = 0;
    int col_start = *column;

    buffer[buf_index++] = '{';
    char c;

    // Lê o restante do comentário
    while ((c = fgetc(file)) != EOF && buf_index < sizeof(buffer) - 1)
    {
        buffer[buf_index++] = c == '\n' ? ' ' : c;
        (*column)++;

        // Atualiza a linha e a coluna
        if (c == '\n')
        {
            new_line(line, column);
        }

        // Se encontrar o fechamento do comentário
        if (c == '}')
        {
            buffer[buf_index] = '\0';
            return new_token(TKN_COMMENT, *line, col_start, buffer);
        }
    }

    buffer[buf_index] = '\0';
    return new_token(TKN_REJECT, *line, col_start, "Comentário não fechado");
}

// Processa atribuições e operadores
Token *process_assignment_and_op(FILE *file, char c, int *line, int *column)
{
    enum TokenType type;
    char nextChar = fgetc(file);
    char buffer[3] = {c, nextChar, '\0'};

    switch (c)
    {
    case '=':
        type = TKN_EQ;
        ungetc(nextChar, file);
        buffer[1] = '\0';
        break;
    case ':':
        if (nextChar == '=')
        {
            type = TKN_ASSIGN;
            (*column)++;
        }
        break;
    case '<':
        if (nextChar == '=')
        {
            type = TKN_LTEQ;
            (*column)++;
        }
        break;
    case '>':
        if (nextChar == '=')
        {
            type = TKN_GTEQ;
            (*column)++;
        }
        break;
    case '!':
        if (nextChar == '=')
        {
            type = TKN_DIFF;
            (*column)++;
        }
        break;
    default:
        ungetc(nextChar, file);
        buffer[1] = '\0';
        type = TKN_REJECT;
        break;
    }

    return new_token(type, *line, *column, buffer);
}

// Automato para validar identificadores
int is_valid_id(char *word)
{
    typedef enum
    {
        START,
        LETTER,
        REJECT
    } State;

    State state = START;
    int i = 0;

    while (word[i] != '\0')
    {
        switch (state)
        {
        case START:
            if (is_letter(word[i]) || word[i] == '_')
            {
                state = LETTER;
            }
            else
            {
                state = REJECT;
            }
            break;
        case LETTER:
            if (is_letter(word[i]) || is_digit(word[i]) || word[i] == '_')
            {
                state = LETTER;
            }
            else
            {
                state = REJECT;
            }
            break;
        case REJECT:
            return 0;
        }

        i++;
    }

    if (i > MAX_ID_SIZE)
    {
        return 0;
    }

    return state == LETTER;
}

// Processa identificadores e palavras reservadas
Token *process_id_or_keyword(FILE *file, char initial, int *line, int *column)
{
    int col_start = *column;
    char *word = read_word(file, initial, column);

    // Verifica se é uma palavra reservada
    if (is_resword(word))
    {
        return new_token(TKN_RESWORD, *line, col_start, word);
    }

    // Verifica se é um identificador válido
    if (is_valid_id(word))
    {
        return new_token(TKN_VAR, *line, col_start, word);
    }

    return new_token(TKN_REJECT, *line, col_start, word);
}

// Automato para validar números inteiros
int is_valid_int(char *word)
{
    typedef enum
    {
        START,
        DIGIT,
        REJECT
    } State;

    State state = START;
    int i = 0;

    while (word[i] != '\0')
    {
        switch (state)
        {
        case START:
            if (is_int_digit(word[i]))
            {
                state = DIGIT;
            }
            else
            {
                state = REJECT;
            }
            break;
        case DIGIT:
            if (is_digit(word[i]))
            {
                state = DIGIT;
            }
            else
            {
                state = REJECT;
            }
            break;
        case REJECT:
            return 0;
        }

        i++;
    }

    if (i > MAX_INT_SIZE)
    {
        return 0;
    }

    return state == DIGIT;
}

// Processa números inteiros
Token *process_integer(FILE *file, char initial, int *line, int *column)
{
    int col_start = *column;
    char *word = read_word(file, initial, column);

    // Verifica se é um número inteiro válido
    if (is_valid_int(word))
    {
        return new_token(TKN_NUMINT, *line, col_start, word);
    }

    return new_token(TKN_REJECT, *line, col_start, word);
}

// Automato para validar números hexadecimais
int is_valid_hex(char *word)
{
    typedef enum
    {
        START,
        HEX,
        REJECT
    } State;

    State state = START;
    int i = 0;

    while (word[i] != '\0')
    {
        switch (state)
        {
        case START:
            if (word[i] == '#')
            {
                state = HEX;
            }
            else
            {
                state = REJECT;
            }
            break;
        case HEX:
            if (is_hex_digit(word[i]))
            {
                state = HEX;
            }
            else
            {
                state = REJECT;
            }
            break;
        case REJECT:
            return 0;
        }

        i++;
    }

    return state == HEX;
}

// Processa números hexadecimais
Token *process_hex(FILE *file, int *line, int *column)
{
    int col_start = *column;
    char *word = read_word(file, '#', column);

    // Verifica se é um número hexadecimal válido
    if (is_valid_hex(word))
    {
        return new_token(TKN_NUMHEX, *line, col_start, word);
    }

    return new_token(TKN_REJECT, *line, col_start, word);
}

// Retorna o próximo token da cadeia de entrada
Token *get_token(FILE *file, int *line, int *column)
{
    char c;

    while ((c = fgetc(file)) != EOF)
    {
        (*column)++;

        // Comentários
        if (c == '{')
        {
            return process_comment(file, line, column);
        }

        // Delimitadores e separadores
        if (is_delimeter(c) || c == ';' || c == '(' || c == ')')
        {
            return process_delimiter_sep_par(file, c, line, column);
        }

        // Atribuição e operadores
        if (is_operator_start(c))
        {
            return process_assignment_and_op(file, c, line, column);
        }

        // Variaveis e palavras reservadas
        if (is_letter(c))
        {
            return process_id_or_keyword(file, c, line, column);
        }

        // Números inteiros
        if (is_int_digit(c))
        {
            return process_integer(file, c, line, column);
        }

        // Números hexadecimais
        if (c == '#')
        {
            return process_hex(file, line, column);
        }

        return new_token(TKN_REJECT, *line, *column, "UNKNOWN");
    }

    // Se alcançar EOF sem encontrar mais tokens
    return NULL;
}

/*HEADER***********************************************************************
 * INSTITUITION  : IFSul - Passo Fundo
 * COURSE/SUBJECT: BCC – Compilers – AT01: Lex Analysis Work
 * DATE          : 2024-04-28
 * FILENAME      : G2-scanner-lib.h
 * DESCRIPTION   :
 *     Scanner implementation for G2
 * AUTHOR(S)     : Luan Daros & Luiz Brugnera
 **************************************************************************END*/
#ifndef G2_SCANNER_H
#define G2_SCANNER_H

#include <stdio.h>

#define MAX_ID_SIZE 30  // Tamanho maximo de um identificador
#define MAX_INT_SIZE 12 // Tamanho maximo de um inteiro
#define BLOCK_SIZE 128  // Tamanho do bloco de leitura

// Tipos de tokens
enum TokenType
{
    TKN_COMMENT,  // Comentario ( {...} )
    TKN_VAR,      // Variavel, identificador
    TKN_RESWORD,  // Palavra reservada
    TKN_LTEQ,     // <=
    TKN_GTEQ,     // >=
    TKN_EQ,       // =
    TKN_DIFF,     // !=
    TKN_ASSIGN,   // :=
    TKN_NUMINT,   // Numero inteiro,
    TKN_NUMHEX,   // Numero hexadecimal (Prefixo #)
    TKN_STMSEP,   // Separador de sentenças (;)
    TKN_LEFTPAR,  // (
    TKN_RIGHTPAR, // )
    TKN_REJECT,   // Cadeias rejeitadas (SP, HT, LF, CR)
};

// Estrutura para representar um token
typedef struct
{
    enum TokenType type; // Tipo do token
    int line;            // Linha onde o token foi encontrado
    int column;          // Coluna inicial do token
    char *lexeme;        // Texto do token
} Token;

// Tokens em formato de string
extern const char *token_type_str[];

// Palavras reservadas
extern const char *reswords[];
extern const int reswords_size;

Token *new_token(enum TokenType type, int line, int col_start, const char *lexeme);

void free_token(Token *token);

Token *get_token(FILE *file, int *line, int *column);

int is_operator_start(char c);

#endif // G2_SCANNER_H
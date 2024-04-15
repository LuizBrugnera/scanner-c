#ifndef G2_SCANNER_H
#define G2_SCANNER_H

#include <stdio.h>

#define MAX_ID_SIZE 30  // Tamanho maximo de um identificador
#define MAX_INT_SIZE 12 // Tamanho maximo de um inteiro

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
    int column_start;    // Coluna de início do token
    int column_end;      // Coluna de fim do token
    char *lexeme;        // Texto do token
} Token;

// Tokens em formato de string
extern const char *token_type_str[];

// Palavras reservadas
extern const char *reswords[];

Token *createToken(enum TokenType type, int line, int col_start, int col_end, const char *lexeme);

void freeToken(Token *token);

Token *getToken(FILE *file);

#endif // G2_SCANNER_H
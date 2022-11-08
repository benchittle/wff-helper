#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

/*
TODO:
- data structures for subwff list, token list
*/

typedef enum {
    WTT_NONE,
    WTT_LPAREN,
    WTT_RPAREN,
    WTT_PROPOSITION,
    WTT_OPERATOR
} WffTokenType;


typedef enum {
    WO_NOT,
    WO_AND,
    WO_OR,
    WO_COND,
    WO_BICOND
} WffOperator;


typedef struct WffToken WffToken;
typedef struct WffParsetree WffParsetree;
typedef struct WffTokenList WffTokenList; // TODO

typedef struct WffOld {
    const char* string;
    WffParsetree* parsetree;
    WffToken* token_array;
    size_t token_count;
} WffOld;

typedef struct Wff {
    char* string;
    struct Wff* subwffs[5];
} Wff;

WffOld* wffold_create(char* wff_string);
void wff_destroy(WffOld* wff);
size_t wff_subwffs(WffOld* wff, WffOld*** subwffs);

/* 
 * Given a string denoting a wff, identify all wff tokens in the string and 
 * store them in order (left to right) in 'tokenArray'.
 * Returns the number of tokens found.
 */
int tokenize_wff(const char* wff, WffToken* tokenArray);

#endif
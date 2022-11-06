#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

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
typedef struct WffParseTree WffParseTree;

typedef struct Wff {
    const char* string;
    WffParseTree* parse_tree;
    WffToken* token_array;
    size_t token_count;
} Wff;

Wff* wff_create(char* wff_string);
void wff_destroy(Wff* wff);
bool wff_validate(Wff* wff);
size_t wff_subwffs(Wff* wff, Wff* subwffs[]);

/* 
 * Given a string denoting a wff, identify all wff tokens in the string and 
 * store them in order (left to right) in 'tokenArray'.
 * Returns the number of tokens found.
 */
int tokenize_wff(const char* wff, WffToken* tokenArray);

#endif
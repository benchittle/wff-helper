#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <string.h>

#include "parsing.h"

/* 
 * A parsing token. If it is of type WTT_PROPOSITION, the 'variable' member will
 * store the letter denoting the propositional variable. Otherwise, if it is of 
 * type WTT_OPERATOR, the 'operator' member will indicate which operator using the 
 * above enum.
 */
struct WffToken {
    WffTokenType type;
    union {
        char variable;
        WffOperator operator;
    };
};


/*
 * A parse tree node. If it is a terminal node, we use the 'token' member to
 * store the token for the terminal symbol. Otherwise, we use the 'child_count'
 * and 'children' members to store pointers to this node's children nodes 
 * (there can be up to 5, depending on the wff). 
 */
typedef struct WffParseTreeNode {
    bool isTerminal;
    union {
        struct {
            int child_count;
            struct WffParseTreeNode* children[5];
        };
        WffToken token;
    };
} WffParseTreeNode;

struct WffParseTree {
    WffParseTreeNode* root;
};

size_t wff_tokenize(char* wff_string, WffToken** token_array_ptr);

bool _wff_validate(WffToken* const tokenArray, const int length, int* index);

WffToken* _wff_next_token(WffToken* const tokenArray, const int length, int* index);


Wff* wff_create(char* wff_string) {
    Wff* wff = malloc(sizeof(Wff));
    wff->string = wff_string;
    wff->parse_tree = NULL;

    WffToken* token_array = NULL;
    size_t token_count = wff_tokenize(wff->string, &token_array);
    
    wff->token_array = token_array;
    wff->token_count = token_count;

    return wff;
}

bool wff_validate(Wff* wff) {
    int index = 0;
    bool result = _wff_validate(wff->token_array, wff->token_count, &index);
    if (index < wff->token_count) {
        return false;
    } else {
        return result;
    }
}

bool _wff_validate(WffToken* const tokenArray, const int length, int* index) {
    int savedIndex = *index; 
    WffToken* next;
    if (_wff_next_token(tokenArray, length, index)->type == WTT_PROPOSITION) {
        return true;
    } 
    *index = savedIndex;
    next = _wff_next_token(tokenArray, length, index);
    if (next->type == WTT_OPERATOR && next->operator == WO_NOT && _wff_validate(tokenArray, length, index)) {
        return true;
    } 
    *index = savedIndex;
    if (_wff_next_token(tokenArray, length, index)->type == WTT_LPAREN && _wff_validate(tokenArray, length, index)) {
        savedIndex = *index;
        next = _wff_next_token(tokenArray, length, index);
        if (next->type == WTT_OPERATOR
                && (next->operator == WO_AND || next->operator == WO_OR || next->operator == WO_COND || next->operator == WO_BICOND)
                && _wff_validate(tokenArray, length, index)
                && _wff_next_token(tokenArray, length, index)->type == WTT_RPAREN) {
            return true;
        }
    }
    return false;
}

WffToken* _wff_next_token(WffToken* const tokenArray, const int length, int* index) {
    if (*index <= length) {
        *index += 1;
        return tokenArray + *index - 1;
    } else {
        return NULL;
    }
}

void wff_destroy(Wff* wff);
char* wff_get_string(Wff* wff);
size_t wff_subwffs(Wff* wff, Wff* subwffs[]);


/* wff_tokenize
 * wff -- Wff to tokenize
 * token_array_ptr -- Address of the caller's pointer to store an array. The 
 *      caller's pointer will be realloc'd and overwritten.
 * Returns the number of tokens in the array starting from *token_array_ptr.
 */
size_t wff_tokenize(char* wff_string, WffToken** token_array_ptr) {
    // Get the pointer to the array and reallocate it with a default size of 32
    // tokens.
    WffToken* token_array = *token_array_ptr;
    size_t array_length = 32;
    token_array = realloc(token_array, array_length * sizeof(*token_array));
    size_t token_count = 0;

    for (char* c = wff_string; *c != '\0'; c++) {
        WffToken token = {.type = WTT_NONE};
        switch (*c) {
            case ' ': break;
            case '~':
                token.type = WTT_OPERATOR;
                token.operator = WO_NOT;
                break;
            case 'v': 
                token.type = WTT_OPERATOR;
                token.operator = WO_OR;
                break;
            case '^':
                token.type = WTT_OPERATOR;
                token.operator = WO_AND;
                break;
            case '=':
                token.type = WTT_OPERATOR;
                token.operator = WO_COND;
                c += 1;
                break;
            case '<':
                token.type = WTT_OPERATOR;
                token.operator = WO_BICOND;
                c += 2;
                break;

            case '(':
                token.type = WTT_LPAREN;
                break;
            case ')':
                token.type = WTT_RPAREN;
                break;

            default:
                if (('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z')) {
                    token.type = WTT_PROPOSITION;
                    token.variable = *c;
                } else {
                    printf("ERROR: Unexpected token: %c\n", *c);
                    return -1;
                }
        } // switch
        
        // If a token was created, append it to the array.
        if (token.type != WTT_NONE) {
            token_array[token_count] = token;
            token_count++;
            // Resize the array if full.
            if (token_count == array_length) {
                array_length *= 2;
                token_array = realloc(token_array, array_length * sizeof(*token_array));
            }
        }
    }
    // Resize the array to the exact number of tokens.
    if (token_count != array_length) {
        token_array = realloc(token_array, token_count * sizeof(*token_array));
    }
    // Update the caller's pointer to the potentially new address of the array.
    *token_array_ptr = token_array;
    return token_count;
}

// v TODO v

void print_token(WffToken* token);

char* get_token_str(WffToken* token);

int tokenize_wff(const char* wff, WffToken* tokenArray);

WffToken* next_token(WffToken* const tokenArray, const int length, int* index);

bool _wff(WffToken* const tokenArray, const int length, int* index);

bool _parse(WffToken* const tokenArray, const int length, int* index, WffParseTreeNode* node);

WffParseTree parse_wff(WffToken* const tokenArray, int length);

bool validate_wff(WffToken* const tokenArray, const int length);

void _print_parsetree(WffParseTreeNode* node, int level);

void print_parsetree(WffParseTree tree);

char* _sub_wffs(WffParseTreeNode* node, char* subwffs[], int* index);

int sub_wffs(WffParseTree tree, char* subwffs[]);

void print_unique(char* subwffs[], int length);

void test_wffs() {
    char* wffs[] = {"~", "v", "^", "=>", "<=>",
                    "p", "q", "pq", "qp",
                    "~p", "(p v q)", "(p ^ q)", "(p => q)", "(p <=> q)",
                    "~(p v q)", "(~p v ~q)", "~(p => ~q)", "~~(~~p ^ ~~q)",
                    "(((p v q) ^ (p v ~q)) => p)", "(p v q) ^ (p v ~q) => p",
                    "(p v q v r)", "(p v q ^ r)"};
    bool results[] = {false, false, false , false, false,
                      true, true, false, false,
                      true, true, true, true, true,
                      true, true, true, true,
                      true, false,
                      false, false
                      };
    printf("Testing wffs...\n");
    int passed = 0;
    int failed = 0;
    for (int i = 0; i < sizeof(wffs) / sizeof(*wffs); i++) {
        char* wff = wffs[i];
        printf("wff %d: '%s'\n", i, wff);

        WffToken tokenArray[100];
        int count = tokenize_wff(wff, tokenArray);
        printf("\t%d tokens\n", count);

        for (int j = 0; j < count; j++) {
            printf("\t\tToken %d: ", j);
            print_token(tokenArray + j);
            printf("\n");
        }

        bool isValid = validate_wff(tokenArray, count);
        if (isValid == results[i]) {
            printf("\tTest passed: result=%d\n", isValid);
            passed++;
        } else {
            printf("\tTest failed: result=%d, should be %d\n", isValid, results[i]);
            failed++;
        }
    }
    printf("%d CASES PASSED\n%d CASES FAILED\n", passed, failed);
}

int main(void) {
    char* wff_string = "(p v q)";

    Wff* wff = wff_create(wff_string);
    printf("%s\n", wff->string);
    for (int j = 0; j < wff->token_count; j++) {
        printf("\t\tToken %d: ", j);
        print_token(&(wff->token_array[j]));
        printf("\n");
    }

    
    /*char* subwffs[100];
    WffToken* tokenArray[100];
    int token_count = tokenize_wff(wff, tokenArray);
    WffParseTree tree = parse_wff(tokenArray, token_count);
    int num_subwffs = sub_wffs(tree, subwffs);
    printf("%d subwffs:\n", num_subwffs);
    print_unique(subwffs, num_subwffs); 
    */
    
    /*for (int i = 0; i < num_subwffs; i++) {
        printf("%d: '%s'\n", i, subwffs[i]);
    }*/
    //print_parsetree(tree);

    printf("done\n");
}

void print_token(WffToken* token) {
    if (token->type == WTT_OPERATOR) {
        switch (token->operator) {
            case WO_NOT: 
                printf("~"); 
                break;
            case WO_AND: 
                printf("^"); 
                break;
            case WO_OR:
                printf("v");
                break;
            case WO_COND:
                printf("=>");
                break;
            case WO_BICOND:
                printf("<=>");
                break;
        }
    } else if (token->type == WTT_PROPOSITION) {
        printf("%c", token->variable);
    } else if (token->type == WTT_LPAREN) {
        printf("(");
    } else {
        printf(")");
    }
}

char* get_token_str(WffToken* token) {
    char* str;
    if (token->type == WTT_OPERATOR) {
        switch (token->operator) {
            case WO_NOT: 
                str = calloc(2, sizeof(char));
                str[0] = '~';
                break;
            case WO_AND: 
                str = calloc(2, sizeof(char));
                str[0] = '^';
                break;
            case WO_OR:
                str = calloc(2, sizeof(char));
                str[0] = 'v';
                break;
            case WO_COND:
                str = calloc(3, sizeof(char));
                str[0] = '=';
                str[1] = '>';
                break;
            case WO_BICOND:
                str = calloc(4, sizeof(char));
                str[0] = '<';
                str[1] = '=';
                str[2] = '>';
                break;
        }
    } else if (token->type == WTT_PROPOSITION) {
        str = calloc(2, sizeof(char));
        str[0] = token->variable;
    } else if (token->type == WTT_LPAREN) {
        str = calloc(2, sizeof(char));
        str[0] = '(';
    } else {
        str = calloc(2, sizeof(char));
        str[0] = ')';
    }
    return str;
}

int tokenize_wff(const char* wff, WffToken* tokenArray) {
    int tokenCount = 0;
    char* c = wff;
    while (*c != '\0') {
        WffToken token = {.type = WTT_NONE};
        switch (*c) {
            case ' ': break;
            case '~':
                token.type = WTT_OPERATOR;
                token.operator = WO_NOT;
                break;
            case 'v': 
                token.type = WTT_OPERATOR;
                token.operator = WO_OR;
                break;
            case '^':
                token.type = WTT_OPERATOR;
                token.operator = WO_AND;
                break;
            case '=':
                token.type = WTT_OPERATOR;
                token.operator = WO_COND;
                c += 1;
                break;
            case '<':
                token.type = WTT_OPERATOR;
                token.operator = WO_BICOND;
                c += 2;
                break;

            case '(':
                token.type = WTT_LPAREN;
                break;
            case ')':
                token.type = WTT_RPAREN;
                break;

            default:
                if (('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z')) {
                    token.type = WTT_PROPOSITION;
                    token.variable = *c;
                } else {
                    printf("ERROR: Unexpected token: %c\n", *c);
                    return -1;
                }
        } // switch
        if (token.type != WTT_NONE) {
            tokenArray[tokenCount] = token;
            tokenCount++;
        }
        c++;
    }
    return tokenCount;
}

WffToken* next_token(WffToken* const tokenArray, const int length, int* index) {
    if (*index <= length) {
        *index += 1;
        return tokenArray + *index - 1;
    } else {
        return NULL;
    }
}

bool _wff(WffToken* const tokenArray, const int length, int* index) {
    int savedIndex = *index; 
    WffToken* next;
    if (next_token(tokenArray, length, index)->type == WTT_PROPOSITION) {
        return true;
    } 
    *index = savedIndex;
    next = next_token(tokenArray, length, index);
    if (next->type == WTT_OPERATOR && next->operator == WO_NOT && _wff(tokenArray, length, index)) {
        return true;
    } 
    *index = savedIndex;
    if (next_token(tokenArray, length, index)->type == WTT_LPAREN && _wff(tokenArray, length, index)) {
        savedIndex = *index;
        next = next_token(tokenArray, length, index);
        if (next->type == WTT_OPERATOR
                && (next->operator == WO_AND || next->operator == WO_OR || next->operator == WO_COND || next->operator == WO_BICOND)
                && _wff(tokenArray, length, index)
                && next_token(tokenArray, length, index)->type == WTT_RPAREN) {
            return true;
        }
    }
    return false;
}

bool _parse(WffToken* const tokenArray, const int length, int* index, WffParseTreeNode* node) {
    //int savedIndex = *index; 
    WffToken* next = next_token(tokenArray, length, index);
    if (next->type == WTT_PROPOSITION) {
        node->isTerminal = false;
        node->child_count = 1;
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        return true;
    } else if (next->type == WTT_OPERATOR && next->operator == WO_NOT) {
        node->isTerminal = false;
        node->child_count = 2;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        return true;
    } else if (next->type == WTT_LPAREN) {
        node->isTerminal = false;
        node->child_count = 5;
        // First child
        WffParseTreeNode* newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParseTreeNode));
        _parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        // Third child
        next = next_token(tokenArray, length, index);
        if (next->type != WTT_OPERATOR || (next->type == WTT_OPERATOR && next->operator != WO_AND && next->operator != WO_OR && next->operator != WO_COND && next->operator != WO_BICOND)) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[2] = newNode;
        // Fourth child
        newNode = malloc(sizeof(WffParseTreeNode));
        _parse(tokenArray, length, index, newNode);
        node->children[3] = newNode;
        // Fifth child
        next = next_token(tokenArray, length, index);
        if (next->type != WTT_RPAREN) {
            return false;
        }
        newNode = malloc(sizeof(WffParseTreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[4] = newNode;
        return true;
    }
    return false;
}

WffParseTree parse_wff(WffToken* const tokenArray, int length) {
    WffParseTree tree;
    WffParseTreeNode* root = malloc(sizeof(WffParseTreeNode));
    int index = 0;

    bool valid = _parse(tokenArray, length, &index, root);
    printf("VALID: %d\n", valid);
    tree.root = root;
    return tree;
}

bool validate_wff(WffToken* const tokenArray, const int length) {
    int index = 0;
    bool result = _wff(tokenArray, length, &index);
    if (index < length) {
        return false;
    } else {
        return result;
    }
}

void _print_parsetree(WffParseTreeNode* node, int level) {
    if (node->isTerminal) {
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        print_token(&(node->token));
        printf("\n");
    } else {
        for (int i = 0; i < node->child_count / 2; i++) {
            _print_parsetree(node->children[i], level + 1);
        }
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("wff\n");
        for (int i = node->child_count / 2; i < node->child_count; i++) {
            _print_parsetree(node->children[i], level + 1);
        }
    }
}

void print_parsetree(WffParseTree tree) {
    _print_parsetree(tree.root, 0);
}

char* _sub_wffs(WffParseTreeNode* node, char* subwffs[], int* index) {
    char* str;
    if (node->isTerminal) {
        str = get_token_str(&(node->token));
        return str;
    } else {
        str = malloc(1);
        int str_size = 1;
        for (int i = 0; i < node->child_count; i++) {
            char* substr = _sub_wffs(node->children[i], subwffs, index);
            str_size += strlen(substr);
            str = realloc(str, str_size);
            strcat(str, substr);
        }
        subwffs[*index] = str;
        *index += 1;
        return str;
    }
}

int sub_wffs(WffParseTree tree, char* subwffs[]) {
    int index = 0;
    _sub_wffs(tree.root, subwffs, &index);
    return index;
}

void print_unique(char* subwffs[], int length) {
    char* done[length];
    int done_count = 0;
    for (int i = 0; i < length; i++) {
        bool isUniqe = true;
        for (int j = 0; j < done_count; j++) {
            if (strcmp(subwffs[i], done[j]) == 0) {
                isUniqe = false;
                break;
            }
        }
        if (isUniqe) {
            printf("%d: '%s'\n", i, subwffs[i]);
            done[done_count] = subwffs[i];
            done_count++;
        }
    }
}
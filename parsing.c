#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
typedef struct WffParsetreeNode {
    bool isTerminal;
    union {
        struct {
            int child_count;
            struct WffParsetreeNode* children[5];
        };
        WffToken token;
    };
} WffParsetreeNode;

struct WffParsetree {
    WffParsetreeNode* root;
};

size_t wff_tokenize(char* wff_string, WffToken** token_array_ptr);
void wff_token_print(WffToken* token);

bool wff_validate(const WffToken* token_array, const size_t token_count);
bool _wff_validate(WffToken* const tokenArray, const int length, int* index);
WffToken* _wff_next_token(WffToken* const tokenArray, const int length, int* index);

WffParsetree* wffold_parse(WffToken* const tokenArray, int length);
bool _wffold_parse(WffToken* const tokenArray, const int length, int* index, WffParsetreeNode* node);
void wff_parsetree_print(WffParsetree* tree);
void _wff_parsetree_print(WffParsetreeNode* node, int level);

char* token_get_string(WffToken* token);
char* _wff_subwffs(WffParsetreeNode* node, char* subwffs[], size_t* index);

// ========



Wff* wff_create(char* wff_string) {
    Wff* wff = malloc(sizeof(Wff));
    wff->string = wff_string;
    
    WffToken* token_array = NULL;
    size_t token_count = wff_tokenize(wff->string, &token_array);
    
    
}

WffOld* wffold_create(char* wff_string) {
    WffOld* wff = malloc(sizeof(WffOld));
    wff->string = wff_string;
    wff->parsetree = NULL;

    WffToken* token_array = NULL;
    size_t token_count = wff_tokenize(wff->string, &token_array);

    wff->token_array = token_array;
    wff->token_count = token_count;

    wff->parsetree = wffold_parse(token_array, token_count);
    if (wff->parsetree == NULL) {
        printf("ERROR: Invalid wff '%s'\n", wff->string);
        exit(1);
    }
    return wff;
}

bool wff_parse(WffToken* const token_array, int length, Wff* wff) {
    int index = 0;

    bool valid = _wffold_parse(token_array, length, &index, wff);
    if (valid && index >= length) {
        return true;
    } else {
        // FREE NODES
        return false;
    }
}


bool _wff_parse(WffToken* const tokenArray, const int length, int* index, Wff* wff) {
    //int savedIndex = *index; 
    WffToken* next = _wff_next_token(tokenArray, length, index);
    if (next->type == WTT_PROPOSITION) {
        node->isTerminal = false;
        node->child_count = 1;
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        return true;
    } else if (next->type == WTT_OPERATOR && next->operator == WO_NOT) {
        node->isTerminal = false;
        node->child_count = 2;
        // First child
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        return true;
    } else if (next->type == WTT_LPAREN) {
        node->isTerminal = false;
        node->child_count = 5;
        // First child
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        // Third child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_OPERATOR || (next->type == WTT_OPERATOR && next->operator != WO_AND && next->operator != WO_OR && next->operator != WO_COND && next->operator != WO_BICOND)) {
            return false;
        }
        newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[2] = newNode;
        // Fourth child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[3] = newNode;
        // Fifth child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_RPAREN) {
            return false;
        }
        newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[4] = newNode;
        return true;
    }
    return false;
}


WffParsetree* wffold_parse(WffToken* const token_array, int length) {
    WffParsetreeNode* root = malloc(sizeof(WffParsetreeNode));
    int index = 0;

    bool valid = _wffold_parse(token_array, length, &index, root);
    if (valid && index >= length) {
        WffParsetree* tree = malloc(sizeof(WffParsetree));
        tree->root = root;
        return tree;
    } else {
        // FREE NODES
        return NULL;
    }
}

bool _wffold_parse(WffToken* const tokenArray, const int length, int* index, WffParsetreeNode* node) {
    //int savedIndex = *index; 
    WffToken* next = _wff_next_token(tokenArray, length, index);
    if (next->type == WTT_PROPOSITION) {
        node->isTerminal = false;
        node->child_count = 1;
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        return true;
    } else if (next->type == WTT_OPERATOR && next->operator == WO_NOT) {
        node->isTerminal = false;
        node->child_count = 2;
        // First child
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        return true;
    } else if (next->type == WTT_LPAREN) {
        node->isTerminal = false;
        node->child_count = 5;
        // First child
        WffParsetreeNode* newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[0] = newNode;
        // Second child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[1] = newNode;
        // Third child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_OPERATOR || (next->type == WTT_OPERATOR && next->operator != WO_AND && next->operator != WO_OR && next->operator != WO_COND && next->operator != WO_BICOND)) {
            return false;
        }
        newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[2] = newNode;
        // Fourth child
        newNode = malloc(sizeof(WffParsetreeNode));
        _wffold_parse(tokenArray, length, index, newNode);
        node->children[3] = newNode;
        // Fifth child
        next = _wff_next_token(tokenArray, length, index);
        if (next->type != WTT_RPAREN) {
            return false;
        }
        newNode = malloc(sizeof(WffParsetreeNode));
        newNode->isTerminal = true;
        newNode->token = *next;
        node->children[4] = newNode;
        return true;
    }
    return false;
}

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

void wff_token_print(WffToken* token) {
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


bool wff_validate(const WffToken* token_array, const size_t token_count) {
    int index = 0;
    bool result = _wff_validate(token_array, token_count, &index);
    if (index < token_count) {
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




void wff_parsetree_print(WffParsetree* tree) {
    _wff_parsetree_print(tree->root, 0);
}

void _wff_parsetree_print(WffParsetreeNode* node, int level) {
    if (node->isTerminal) {
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        wff_token_print(&(node->token));
        printf("\n");
    } else {
        for (int i = 0; i < node->child_count / 2; i++) {
            _wff_parsetree_print(node->children[i], level + 1);
        }
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("wff\n");
        for (int i = node->child_count / 2; i < node->child_count; i++) {
            _wff_parsetree_print(node->children[i], level + 1);
        }
    }
}


char* token_get_string(WffToken* token) {
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

size_t wff_subwffs(WffOld* wff, WffOld*** subwffs_ptr) {
    size_t index = 0;
    WffOld** subwffs = *subwffs_ptr;
    char* subwff_strs[100];
    _wff_subwffs(wff->parsetree->root, subwff_strs, &index);
    
    subwffs = realloc(subwffs, index * sizeof(WffOld*));
    for (int i = 0; i < index; i++) {
        subwffs[i] = wffold_create(subwff_strs[i]);
    }
    *subwffs_ptr = subwffs;
    return index;
}

char* _wff_subwffs(WffParsetreeNode* node, char* subwffs[], size_t* index) {
    char* str;
    if (node->isTerminal) {
        str = token_get_string(&(node->token));
        return str;
    } else {
        str = malloc(1);
        int str_size = 1;
        for (int i = 0; i < node->child_count; i++) {
            char* substr = _wff_subwffs(node->children[i], subwffs, index);
            str_size += strlen(substr);
            str = realloc(str, str_size);
            strcat(str, substr);
        }
        subwffs[*index] = str;
        *index += 1;
        return str;
    }
}

void print_unique(WffOld** subwffs, size_t length) {
    char* done[length];
    int done_count = 0;
    for (int i = 0; i < length; i++) {
        bool isUniqe = true;
        for (int j = 0; j < done_count; j++) {
            if (strcmp(subwffs[i]->string, done[j]) == 0) {
                isUniqe = false;
                break;
            }
        }
        if (isUniqe) {
            printf("%d: '%s'\n", i, subwffs[i]->string);
            done[done_count] = subwffs[i]->string;
            done_count++;
        }
    }
}


// TODO: We need to identify a list of all matches for a given equivalence
// - need a linked list?
// - what is an equivalence doing ?= swapping out one subwff (subtree) for an
//   equivalent one
//      - part 1: identify one or more subwffs (subtrees) matching our rule
//      - part 2: make the substition in one subwff
// Think about making Wff a tree itself 

WffParsetreeNode* _wff_e12(WffParsetreeNode* node) {
    if (node->isTerminal) {
        return NULL;
    }
    if (node->child_count == 5) {
        WffParsetreeNode* child = node->children[2];
        if (child->isTerminal && child->token.type == WTT_OPERATOR && child->token.operator == WO_AND) {
            return node;
        }
    }
}

WffOld* wff_e12(WffOld* wff) {
    
}


/*
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
*/

int main(void) {
    char* wff_string = "((p ^ q) v r)";
    WffOld* wff = wffold_create(wff_string);
    if (wff == NULL) {
        printf("AHHHH\n");
    }
    printf("%s\n", wff->string);
    for (int j = 0; j < wff->token_count; j++) {
        printf("\t\tToken %d: ", j);
        wff_token_print(&(wff->token_array[j]));
        printf("\n");
    }
    wff_parsetree_print(wff->parsetree);
    WffOld** subwffs = NULL;
    size_t subwff_count = wff_subwffs(wff, &subwffs);
    print_unique(subwffs, subwff_count);



    
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








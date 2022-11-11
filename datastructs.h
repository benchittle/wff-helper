#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_

#include "parsing.h"


typedef struct WffList WffList;

WffList* wff_list_create();
void wff_list_destroy(WffList* list);
void wff_list_append(WffList* list, Wff* wff);
void wff_list_reset_current(WffList* list);
Wff* wff_list_next(WffList* list);
size_t wff_list_length(WffList* list);


typedef struct WffTokenList WffTokenList;

WffTokenList* wff_token_list_create();
void wff_token_list_destroy(WffTokenList* list);
void wff_token_list_append(WffTokenList* list, WffToken* wff);
void wff_token_list_reset_current(WffTokenList* list);
WffToken* wff_token_list_next(WffTokenList* list);
size_t wff_token_list_length(WffTokenList* list);

#endif
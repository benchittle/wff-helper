#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "wff-helper.h"
#include "datastructs.h"

/*
TODO:
[x] change WffToken to use WffVar struct instead of char
[x] change earlier functions that used arrays (e.g. token_array) to use list
[ ] abstractify wff_tree (?), parse tree... and their operations
[ ] clean memory leaks
[ ] specify consts, etc. in functions
[ ] make structs and their usage consistent (i.e. pointers to everything)
[ ] iterator function?
[ ] should get_string functions return actual pointer or copy?

*/


int main(void) {
    test();

    printf("done\n");
}








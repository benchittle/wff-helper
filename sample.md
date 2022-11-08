# Examples

## Example 1: Happy Path

Start of proof
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. [show preview of result based on what user has typed]





> E14 1
```

User types valid action 
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. 





> E1 2
```

User types valid action 
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. p v F                           (E1, 2)
4.




> E6 3
```

User types valid action
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (p v q) ^ (p v ~q)              (hypothesis)
2. p v (q ^ ~q)                    (E14, 1)
3. p v F                           (E1, 2)
4. p                               (E6, 3)




PROOF DONE
```

## Example 2: Ambiguous Commands
Start of proof
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)              (hypothesis)
2. [show preview of result based on what user has typed]





> E10 1
```

There is ambiguity with the previous action: which pair of symbols should we apply E10 to? We should allow the user to scroll through possible outcomes, or type in the desired wff manually. Typing will be complicated: we will have to check if the user's wff is one of the valid outcomes. If not, we must tell them. Autocomplete would be neat with this, but not necessary. Typing could also be optional at first. Assume the user scrolls to their desired usage here:
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

Result of scroll to right:
(the change should be highlighted)
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (q v p) ^ (~q v p)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

Another scroll to the right should loop back to the first option:
(the change should be highlighted)
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)





AMBIGUOUS COMMAND: Scroll using arrow keys, press enter to select
> E10 1
```

User presses enter
```
PROVE (p v q) ^ (p v ~q) => p
(Direct Proof)
1. (q v p) ^ (p v ~q)                (hypothesis)
2. (p v q) ^ (p v ~q)                    (E10, 1)
3. 





>
```

# Requirements

## Parsing
We need to be able to tokenize and parse wffs

For example given the wff `(p v q) ^ (p v ~q) => p`, we need to convert it into a series of tokens:
```
tokenize_wff(wff)

OUTPUT
{LPAREN, proposition(p), operator(v), proposition(q), RPAREN, operator(^), LPAREN, proposition(p), operator(v), operator(~), proposition(q), RPAREN, operator(=>), proposition(p)}
```

For example given the wff `(p v q) ^ (p v ~q) => p`, we need a way to split this into all the possible sub-wffs:
```
sub_wffs(wff)

OUTPUT:
{"p", "q", "p v q", "~q", "p v ~q", "(p v q) ^ (p v ~q)", "(p v q) ^ (p v ~q) => p"}
```

Grammar
```
proposition ::= [a-zA-Z]

wff ::= T 
      | F
      | proposition
      | ~wff 
      | (wff ^ wff) 
      | (wff v wff) 
      | (wff => wff) 
      | (wff <=> wff)

====
proposition ::= [a-zA-Z]

bioperator ::= ^ | v | => | <=>

unarywff ::= ~wff

binarywff ::= (wff bioperator wff)

wff ::= proposition
      | unarywff
      | binarywff
```

## Applying Equivalences
Given a wff and an equivalence, we need to determine all possible resulting wffs

For example, given the wff `(p v q) ^ (p v ~q)` if we want to apply E14:
```
equivalent_wffs(wff, equivalence)

OUTPUT:
{"p v (q ^ ~q)"}
```
Alternatively, given the wff `(p v q) ^ (p v ~q)`, suppose we want to apply E10:
```
equivalent_wffs(wff, equivalence)

OUTPUT:
{"(q v p) ^ (p v ~q)", "(p v q) ^ (~q v p)"}
```

### Wff Searching

To do this, we need to define a pattern for each equivalence to be able to identify potential applications of the equivalence. For example, we could define E10 as `"(p v q)"` which could then be passed to a search function:
```
wff_search(wff="((p v q) v (p v p))", search="(p v q)")

OUTPUT
{"(p v q)", "(p v p)", "((p v q) v (p v p))"}
```
It may be more beneficial to have the output be pointers to the subwffs rather than strings. 

A few notes about the search function:
- The letters used in the search wff should be treated as non-terminals and should *not* be confused with terminal propositions in the actual wff. I.e. in the example above, we identified `(p v p)` as one of the matching wffs using the search expression `(p v q)`. 
    - However, if we use the same variable more than once in the search expression, this should be reflected in the results. This may make things difficult (a **wff comparison function** may be needed, or perhaps it could be done iteratively / recursively during the search).
        ```
        wff_search(wff="((p v q) v (p v p))", search="(p v p)")

        OUTPUT
        {"(p v p)"}
        ```
- The returned subwffs should use some kind of handle or pointer to indicate which symbols were matched by each part of the search expression. For example:
    ```
    wff_search(wff="((p v q) v (p v p))", search="(a v b)")

    OUTPUT
    {"(p v q)", "(p v p)", "((p v q) v (p v p))"}
    ```
    We should know that a=`p` and b=`q` in the first case, a=`p` and b=`p` in the second case, and a=`(p v q)` and b=`(p v p)` in the third case.

### Wff Substitution
Once we have identified the (sub)wff to which we want to apply the equivalence, we must 

If a tree is used to store a wff and its corresponding subwffs, we can make a substitution by simply replacing the appropriate subtree with a new one for the equivalent wff.
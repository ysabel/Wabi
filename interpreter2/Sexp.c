#include "Sexp.h"

#include <stdio.h>
#include <stdlib.h>

#include "Environment.h"

Sexp t = { Symbol, .Symbol = { 1, "t" } };
Sexp nil = { Symbol, .Symbol = { 3, "nil" } };

const char *text(Sexp *sexp) {
    if (0 == sexp) { return "nil"; }
    if (Symbol == sexp->type) {
        /* Might not be safe */
        return sexp->Symbol.text;
    }
    /* For now */
    return "";
}
    
void print(Sexp *sexp) {
    if (0 == sexp) {
        printf("nil");
        return;
    }
    
    switch(sexp->type) {
    case Integer:
        printf("%ld", sexp->Integer);
        break;
    case Float:
        printf("%g", sexp->Float);
        break;
    case Symbol:
        for (size_t c = 0; c < sexp->Symbol.length; ++c) { putchar(sexp->Symbol.text[c]); }
        break;
    case Cons:
        putchar('(');
        print(sexp->Cons.car);
        putchar(' ');
        putchar('.');
        putchar(' ');
        print(sexp->Cons.cdr);
        putchar(')');
        break;
    case Function:
        printf("%p{Function}", sexp->Function);
        break;
    default:
        printf("{unknown: %d}", sexp->type);
        break;
    }
}

void dump(Sexp *sexp) {
    if (0 == sexp) {
        printf("{nullptr}nil");
        return;
    }
    
    switch(sexp->type) {
    case Integer:
        printf("%ld{Integer}", sexp->Integer);
        break;
    case Float:
        printf("%g{Float}", sexp->Float);
        break;
    case Symbol:
        for (size_t c = 0; c < sexp->Symbol.length; ++c) { putchar(sexp->Symbol.text[c]); }
        break;
    case Cons:
        putchar('(');
        dump(sexp->Cons.car);
        putchar(' ');
        putchar('.');
        putchar(' ');
        dump(sexp->Cons.cdr);
        putchar(')');
        break;
    case Function:
        printf("%p{Function}", sexp->Function);
        break;
    default:
        printf("{unknown: %d}", sexp->type);
        break;
    }
}

Sexp *eval(Sexp *sexp, Environment *environment) {
    if (0 == sexp) { return &nil; }
    
    Sexp *function = &nil;
    switch (sexp->type) {
    case Integer:
    case Float:
        return sexp;
    case Symbol:
        return environmentLookup(environment, sexp);
    case Cons:
        function = environmentLookup(environment, sexp->Cons.car);
        if (function->type != Function) {
            fprintf(stderr, "Couldn't eval '%s' as function", text(sexp));
            return &nil;
        }
        return function->Function(sexp->Cons.cdr);
    default:
        fprintf(stderr, "Unimplemented eval on Sexp type %d.\n", sexp->type);
        break;
    }
    
    return &nil;
}

Sexp *allocateSexp() { return malloc(sizeof(Sexp)); }
void freeSexp(Sexp **sexp) {
    if (0 != *sexp && &t != *sexp && &nil != *sexp) {
        free(*sexp);
        *sexp = &nil;
    }
}


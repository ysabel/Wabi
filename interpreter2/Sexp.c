#include "Sexp.h"

#include <stdio.h>

Sexp t = { Symbol, .Symbol = { 1, "t" } };
Sexp nil = { Symbol, .Symbol = { 3, "nil" } };

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
    default:
        printf("{unknown: %d}", sexp->type);
        break;
    }
}

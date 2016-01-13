#pragma once
#include <stddef.h>

typedef enum SexpT { Integer, Float, Symbol, Cons } SexpT;
typedef struct Sexp {
    SexpT type;
    union {
        long Integer;
        float Float;
        struct {
            size_t length;
            const char *text;
        } Symbol;
        struct {
            struct Sexp *car;
            struct Sexp *cdr;
        } Cons;
    };
} Sexp;

extern Sexp t, nil;

void print(Sexp *sexp);
void dump(Sexp *sexp);

inline Sexp mkinteger(long value) {
    Sexp sexp = { Integer, .Integer = value };
    return sexp;
}
inline Sexp mkfloat(float value) {
    Sexp sexp = { Float, .Float = value };
    return sexp;
}
inline Sexp mksymbol(size_t length, const char *text) {
    Sexp sexp = { Symbol, .Symbol = { length, text } };
    return sexp;
}
inline Sexp mkcons(Sexp *car, Sexp *cdr) {
    Sexp sexp = { Cons, .Cons = { car, cdr } };
    return sexp;
}

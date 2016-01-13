#pragma once
#include <stddef.h>
#include <string.h>

typedef enum SexpT { Integer, Float, Symbol, Cons, Function } SexpT;
struct Sexp;
typedef struct Sexp *(*FunctionPointer)(struct Sexp *);
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
        FunctionPointer Function;
    };
} Sexp;

extern Sexp t, nil;

const char *text(Sexp *sexp);
void print(Sexp *sexp);
void dump(Sexp *sexp);

/* Forward declaration */
struct Environment;
Sexp *eval(Sexp *sexp, struct Environment *environment);

Sexp *allocateSexp();
void freeSexp(Sexp **sexp);

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
inline Sexp mksymbol0(const char *text) {
    Sexp sexp = { Symbol, .Symbol = { strlen(text), text } };
    return sexp;
}
inline Sexp mkcons(Sexp *car, Sexp *cdr) {
    Sexp sexp = { Cons, .Cons = { car, cdr } };
    return sexp;
}
inline Sexp mkfunction(FunctionPointer function) {
    Sexp sexp = { Function, .Function = function };
    return sexp;
}

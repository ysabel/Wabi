#include <stdio.h>

#include "Parse.h"
#include "Sexp.h"

int ungetchar(int ch) { return ungetc(ch, stdin); }

int main(int argc, char *argv[]) {
    Sexp *sexp;
    do {
        putchar('>');
        putchar(' ');
        sexp = read(getchar, ungetchar);
        putchar('\n');
        dump(sexp);
        putchar('\n');
    } while (0 != sexp);
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Parse.h"
#include "Sexp.h"

int ungetchar(int ch) { return ungetc(ch, stdin); }

int main(int argc, char *argv[]) {
    if (isatty(fileno(stdin))) {
        resetInput("interactive");
    } else {
        resetInput("stdin");
    }
    
    Sexp *sexp;
    do {
        putchar('>');
        putchar(' ');
        sexp = readSexp(getchar, ungetchar);
        putchar('\n');
        dump(sexp);
        putchar('\n');
        if (0 != sexp && &nil != sexp) { free(sexp); }
        if (ferror(stdin)) {
            perror("Error reading sexps");
            break;
        }
    } while (0 != sexp);
    
    return 0;
}

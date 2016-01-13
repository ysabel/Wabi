#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Environment.h"
#include "Parse.h"
#include "Sexp.h"

int ungetchar(int ch) { return ungetc(ch, stdin); }

int main(int argc, char *argv[]) {
    initializeGlobalNamespace();
    
    if (isatty(fileno(stdin))) {
        resetInput("interactive");
    } else {
        resetInput("stdin");
    }
    
    Sexp *sexp, *result;
    do {
        putchar('>');
        putchar(' ');
        sexp = readSexp(getchar, ungetchar);
        putchar('\n');
#ifdef ebugSexps
        dump(sexp);
        putchar('\n');
#endif
        result = eval(sexp, &GlobalNamespace);
        print(result);
        putchar('\n');
        freeSexp(&sexp);
        freeSexp(&result);
        
        if (ferror(stdin)) {
            perror("Error reading sexps");
            break;
        }
    } while (0 != sexp);
    
    return 0;
}

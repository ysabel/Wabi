#include "Environment.h"

#include <stdlib.h>
#include <string.h>

static const size_t EnvironmentPageSize = 1024;

Environment GlobalNamespace;

static void increaseSize(Environment *environment) {
    environment->max += EnvironmentPageSize;
    environment->entries = realloc(environment->entries, environment->max * sizeof(EnvironmentEntry));
}

void environmentInitialize(Environment *environment) {
    environment->length = 0;
    environment->max = EnvironmentPageSize;
    environment->entries = malloc(EnvironmentPageSize * sizeof(EnvironmentEntry));
    memset(environment->entries, 0, EnvironmentPageSize * sizeof(EnvironmentEntry));
}

void environmentAdd(Environment *environment, Sexp *symbol, Sexp *sexp) {
    if (environment->length >= environment->max) { increaseSize(environment); }
    Sexp *name = allocateSexp();
    name->type = Symbol;
    name->Symbol.length = symbol->Symbol.length;
    char *buffer = malloc(symbol->Symbol.length + 1);
    memcpy(buffer, symbol->Symbol.text, symbol->Symbol.length);
    /* Null-terminate for now so we can use strcmp */
    buffer[name->Symbol.length] = '\0';
    name->Symbol.text = buffer;
    environment->entries[environment->length].symbol = name;
    environment->entries[environment->length++].sexp = sexp;
}

Sexp *environmentLookup(Environment *environment, Sexp *symbol) {
    /* Null-terminate for now so we can use strcmp */
    char name[symbol->Symbol.length + 1];
    memcpy(&name, symbol->Symbol.text, symbol->Symbol.length);
    name[symbol->Symbol.length] = '\0';
    
    for (size_t i = 0; i < environment->length; ++i) {
        EnvironmentEntry *entry = &(environment->entries[i]);
        if (symbol->Symbol.length == entry->symbol->Symbol.length &&
            0 == strcmp(name, entry->symbol->Symbol.text)) {
            return entry->sexp;
        }
    }
    return &nil;
}

static Sexp *quote(Sexp *argument) {
    if (Cons == argument->type && &nil == argument->Cons.cdr) {
        return argument->Cons.car;
    }
    return argument;
}

void initializeGlobalNamespace() {
    environmentInitialize(&GlobalNamespace);
    Sexp name;
    Sexp *fp;
    
    name = mksymbol0("quote");
    fp = allocateSexp();
    *fp = mkfunction(&quote);
    environmentAdd(&GlobalNamespace, &name, fp);

    name = mksymbol0("nil");
    environmentAdd(&GlobalNamespace, &name, &nil);
    name = mksymbol0("t");
    environmentAdd(&GlobalNamespace, &name, &t);
}

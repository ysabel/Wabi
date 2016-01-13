#pragma once

#include <stddef.h>

#include "Sexp.h"

typedef struct EnvironmentEntry {
    Sexp *symbol;
    Sexp *sexp;
} EnvironmentEntry;

typedef struct Environment {
    size_t length;
    size_t max;
    EnvironmentEntry *entries;
} Environment;

extern Environment GlobalNamespace;
void initializeGlobalNamespace();

void environmentInitialize(Environment *environment);
void environmentAdd(Environment *environment, Sexp *symbol, Sexp *sexp);
Sexp *environmentLookup(Environment *environment, Sexp *symbol);

#pragma once
#include "Sexp.h"

typedef int(*CharGetter)();
typedef int(*CharUngetter)(int);

void resetInput(const char *input);
Sexp *readSexp(CharGetter getNextChar, CharUngetter ungetNextChar);

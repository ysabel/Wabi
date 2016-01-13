#pragma once
#include "Sexp.h"

typedef int(*CharGetter)();
typedef int(*CharUngetter)(int);

Sexp *read(CharGetter getNextChar, CharUngetter ungetNextChar);

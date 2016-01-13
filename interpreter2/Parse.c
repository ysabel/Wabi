#include "Parse.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token {
    size_t length;
    const char *text;
} Token;

int isstart(Token *token) { return 1 == token->length && '(' == token->text[0]; }
int isend(Token *token) { return 1 == token->length && ')' == token->text[0]; }

int readToken(CharGetter getNextChar, CharUngetter ungetNextChar, Token *token, char **character) {
    int done = 0;
    char *preread = *character;
    char *start = 0;
    while (!done) {
        char *current = *character;
        int next = getNextChar();
        if (0 > next) {
            if (0 != start) {
                token->length = current - start;
                token->text = start;
            }
            return next;
        }
        *current = next;
        ++(*character);

        switch (next) {
        case '(':
        case ')':
        case '\'':
            if (0 != start) {
                ungetNextChar(next);
                token->length = current - start;
                token->text = start;
            } else {
                token->length = 1;
                token->text = current;
            }
            done = 1;
            break;
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
        case ' ':
            if (0 != start) {
                token->length = current - start;
                token->text = start;
                done = 1;
            }
            break;
        default:
            if (0 == start) { start = current; }
            break;
        }
    }

    return *character - preread;
}

Sexp *makeAtom(Token *token) {
    Sexp *atom = malloc(sizeof(Sexp));
    
    // For most of these, we'll need a null terminated string.
    // Go ahead and make that, and then reuse it in the Sexp if necessary.
    char *end;
    char *buffer = malloc(token->length + 1);
    memcpy(buffer, token->text, token->length);
    buffer[token->length] = 0;

    if ((!isdigit(token->text[0]) &&
         '-' != token->text[0] &&
         '+' != token->text[0]) ||
        (1 == token->length &&
         ('-' == token->text[0] ||
          '+' == token->text[0]))) {
        atom->type = Symbol;
        atom->Symbol.length = token->length;
        atom->Symbol.text = buffer;
        return atom;
    }

    if ('0' == token->text[0] && 1 < token->length) {
        const char *base;
        atom->type = Integer;
        if ('x' == token->text[1] || 'X' == token->text[1]) {
            // Hex constant?
            atom->Integer = strtol(buffer, &end, 16);
            base = "hex";
        } else if ('b' == token->text[1] || 'B' == token->text[1]) {
            // Binary constant?
            atom->Integer = strtol(buffer + 2, &end, 2);
            base = "binary";
        } else {
            // Octal constant?
            atom->Integer = strtol(buffer, &end, 8);
            base = "octal";
        }
        if (token->length != end - buffer) {
            fprintf(stderr, "Couldn't parse token '%s' as %s constant.", buffer, base);
            free(atom);
            atom = &nil;
        }
        free(buffer);
        return atom;
    }
    
    // See if there's a decimal point anywhere
    for (size_t c = 0; c < token->length; ++c) {
        if ('.' == token->text[c]) {
            atom->type = Float;
            atom->Float = strtof(buffer, &end);
            if (token->length != end - buffer) {
                fprintf(stderr, "Couldn't parse token '%s' as float.", buffer);
                free(atom);
                atom = &nil;
            }
            free(buffer);
            return atom;
        }
    }

    atom->type = Integer;
    atom->Integer = strtol(buffer, &end, 10);
    if (token->length != end - buffer) {
        fprintf(stderr, "Couldn't parse token '%s' as integer.", buffer);
        free(atom);
        atom = &nil;
    }
    free(buffer);
    return atom;
}

Sexp *readFromTokens(Token *first, Token *last) {
    // Handle individual atoms
    if (1 == (last - first)) {
        return makeAtom(first);
    }

    // NOTE: we assume we only have balanced parens from here on out,
    // since the tokenizer should only give us balanced parens.
    
    // Anything else is a list or cons cell

    // We should be starting with a start token and ending with an end
    // token at this point.
    if (!isstart(first) || !isend(last - 1)) { return &nil; }

    Token *next = first + 1;

    Sexp *head = malloc(sizeof(Sexp));
    Sexp *node = head;
    while (next < (last - 1)) {
        node->type = Cons;
        if (isstart(next)) {
            Token *subfirst = next;
            Token *sublast = next;
            unsigned int depth = 0;
            do {
                if (isstart(sublast)) { ++depth; }
                if (isend(sublast)) { --depth; }
                ++sublast;
            } while (0 < depth);
            node->Cons.car = readFromTokens(subfirst, sublast);
            next = sublast;
        } else {
            node->Cons.car = makeAtom(next);
            ++next;
        }
        if (next < (last - 1)) {
            node->Cons.cdr = malloc(sizeof(Sexp));
            node = node->Cons.cdr;
        } else {
            node->Cons.cdr = &nil;
            node = &nil;
        }
    }
    return head;
}

Sexp *read(CharGetter getNextChar, CharUngetter ungetNextChar) {
    const size_t CharBuffer = 8192;
    const size_t TokenBuffer = 255;

    char characters[CharBuffer];
    Token tokens[TokenBuffer];
    unsigned int depth = 0;
    char *character = characters;
    Token *token = tokens;
    do {
        int status = readToken(getNextChar, ungetNextChar, token, &character);
        if (0 > status) {
            fprintf(stderr, "readToken() returned %d\n", status);
            return 0;
        }
        
        if (isstart(token)) { ++depth; }
        if (isend(token)) { --depth; }
        ++token;
    } while (0 < depth && token < tokens + TokenBuffer);

    if (0 < depth) {
        // Ran out of buffer space before we got an sexp, print an
        // error.
        fprintf(stderr, "Ran out of buffer space before we got a complete sexp, discarding.");
        return &nil;
    }

    printf("%p %p\n", tokens, token);

    for (Token *t = tokens; t < token; ++t) {
        printf("Token: %ld '", t->length);
        for (size_t l = 0; l < t->length; ++l) { putchar(t->text[l]); }
        printf("'\n");
    }

    return readFromTokens(tokens, token);
}

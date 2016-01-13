#include "Parse.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token {
    size_t length;
    const char *text;
} Token;

static const char *parserReadingFrom = "interactive";
static unsigned int line, column;

void resetInput(const char *input) {
    parserReadingFrom = input;
    line = 0;
    column = 0;
}

static void parseError(const char *format, ...) {
    fprintf(stderr, "%s:%d:%02d: ", parserReadingFrom, line, column);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

int isstart(Token *token) { return 1 == token->length && '(' == token->text[0]; }
int isend(Token *token) { return 1 == token->length && ')' == token->text[0]; }
int isdot(Token *token) { return 1 == token->length && '.' == token->text[0]; }
int isquote(Token *token) { return 1 == token->length && '\'' == token->text[0]; }

int readToken(CharGetter getNextChar, CharUngetter ungetNextChar, Token *token, char **character) {
    int done = 0;
    char *preread = *character;
    char *start = 0;
    while (!done) {
        char *current = *character;
        int next = getNextChar();
        ++column;
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
                --column;
                token->length = current - start;
                token->text = start;
            } else {
                token->length = 1;
                token->text = current;
            }
            done = 1;
            break;
        case '\n':
        case '\r':
        case '\f':
            column = 0;
            ++line;
            /* Intentional fallthrough */
        case '\t':
        case '\v':
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
            parseError("Warning: Couldn't parse token '%s' as %s constant, using %d.\n", buffer, base, atom->Integer);
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
                parseError("Warning: Couldn't parse token '%s' as float, using %f.\n", buffer, atom->Float);
            }
            free(buffer);
            return atom;
        }
    }

    atom->type = Integer;
    atom->Integer = strtol(buffer, &end, 10);
    if (token->length != end - buffer) {
        parseError("Warning: Couldn't parse token '%s' as integer, using %d.\n", buffer, atom->Integer);
    }
    free(buffer);
    return atom;
}

Sexp *readFromTokens(Token *first, Token *last);

Sexp *readSubexpression(Token *first, Token **last) {
    unsigned int depth = 0;
    do {
        if (isstart(*last)) { ++depth; }
        if (isend(*last)) { --depth; }
        ++(*last);
    } while (0 < depth);
    return readFromTokens(first, *last);
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
    while (node != &nil && next < (last - 1)) {
        node->type = Cons;
        if (isstart(next)) {
            node->Cons.car = readSubexpression(next, &next);
        } else {
            node->Cons.car = makeAtom(next);
            ++next;
        }

        if (next < (last - 2) && isdot(next)) {
            if (node != head) {
                parseError("Error: Cons cell with multiple sexps before the ., truncating list.\n");
                node->Cons.cdr = &nil;
            } else {
                ++next;
                node->Cons.cdr = readSubexpression(next, &next);
                if (!isend(next)) {
                    parseError("Error: Cons cell with multiple sexps after the ., truncating list.\n");
                    node->Cons.cdr = &nil;
                }
            }
            node = &nil;
        } else {
            if (next < (last - 1)) {
                node->Cons.cdr = malloc(sizeof(Sexp));
                node = node->Cons.cdr;
            } else {
                node->Cons.cdr = &nil;
                node = &nil;
            }
        }
    }
    return head;
}

int readBalancedTokens(CharGetter getNextChar, CharUngetter ungetNextChar,
                       char **buffer, char *bufferEnd,
                       Token **token, const Token *lastToken) {
    unsigned int depth = 0;
    do {
        int status = readToken(getNextChar, ungetNextChar, *token, buffer);
        if (0 > status) {
            return status;
        }
        
        if (isstart(*token)) { ++depth; }
        if (isend(*token)) {
            if (0 < depth) { --depth; }
            else {
                parseError("Error: Extra ) found, discarding.\n");
                continue;
            }
        }
        if (isquote(*token)) {
            (*token)->text = "(";
            ++(*token);
            (*token)->length = 5;
            (*token)->text = "quote";
            ++(*token);
            status = readBalancedTokens(getNextChar, ungetNextChar,
                                        buffer, bufferEnd,
                                        token, lastToken);
            (*token)->length = 1;
            (*token)->text = ")";
            if (0 != status) { return status; }
        }
        ++(*token);
    } while (0 < depth && (*token) < lastToken && (*buffer) < bufferEnd);
    return depth;
}

Sexp *readSexp(CharGetter getNextChar, CharUngetter ungetNextChar) {
    const size_t CharBuffer = 8192;
    const size_t TokenBuffer = 255;

    char characters[CharBuffer];
    Token tokens[TokenBuffer];
    char *character = characters;
    Token *token = tokens;
    int status = readBalancedTokens(getNextChar, ungetNextChar,
                                    &character, characters + CharBuffer,
                                    &token, tokens + TokenBuffer);
    if (0 < status) {
        // Ran out of buffer space before we got an sexp, print an
        // error.
        parseError("Error: Ran out of %s buffer space before we got a complete sexp, discarding.\n",
                   (character < characters + CharBuffer ? "token" : "character"));
        return &nil;
    } else if (0 > status) {
        return 0;
    }

#ifdef ebugTokens
    for (Token *t = tokens; t < token; ++t) {
        printf("Token: %ld '", t->length);
        for (size_t l = 0; l < t->length; ++l) { putchar(t->text[l]); }
        printf("'\n");
    }
#endif

    return readFromTokens(tokens, token);
}

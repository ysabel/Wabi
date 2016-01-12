#include "Sexp.h"

using namespace std;
using namespace Wabi;

namespace Wabi {
    Atom *makeAtom(const Token &token) {
        if ((!isdigit(token.text[0]) &&
             '-' != token.text[0] &&
             '+' != token.text[0]) ||
            (1 == token.length &&
             ('-' == token.text[0] ||
              '+' == token.text[0]))) {
            return new Symbol(token.length, token.text);
        }

        if ('0' == token.text[0]) {
            if ('x' == token.text[1] || 'X' == token.text[1]) {
                // Hex constant?
                string t(token.text, token.length);
                size_t next;
                long value = stol(t, &next, 16);
                if (token.length != next) {
                    throw syntax_error("Couldn't parse token '" + t + "' as hex constant.");
                }
                return new Integer(value);
            }

            if ('b' == token.text[1] || 'B' == token.text[1]) {
                // Binary constant?
                string t(token.text + 2, token.length - 2);
                size_t next;
                long value = stol(t, &next, 2);
                if (token.length - 2 != next) {
                    throw syntax_error("Couldn't parse token '" + t + "' as binary constant.");
                }
                return new Integer(value);
            }
            
            // Octal constant?
            string t(token.text, token.length);
            size_t next;
            long value = stol(t, &next, 8);
            if (token.length != next) {
                throw syntax_error("Couldn't parse token '" + t + "' as octal constant.");
            }
            return new Integer(value);
        }                        

        // See if there's a decimal point anywhere
        for (size_t c = 0; c < token.length; ++c) {
            if ('.' == token.text[c]) {
                string t(token.text, token.length);
                size_t next;
                float value = stof(t, &next);
                if (token.length != next) {
                    throw syntax_error("Couldn't parse token '" + t + "' as float.");
                }
                return new Float(value);
            }
        }
        
        string t(token.text, token.length);
        size_t next;
        long value = stol(t, &next);
        if (token.length != next) {
            throw syntax_error("Couldn't parse token '" + t + "' as integer.");
        }
        return new Integer(value);
    }

}

unique_ptr<Sexp> List::eval(Environment& environment) const {
    // TODO: catch special cases for if, define and quote here
    // Alternatively, make them macros?  Need an impl for macros
    // first.
    try {
        unique_ptr<Sexp> function(unique_ptr<Sexp>(environment.at(*(head()->car.get()))->copy()));
        function->bind(dynamic_cast<const Cons *>(head()->cdr.get()), environment);
        return function->eval(environment);
    } catch (const std::out_of_range &) {
        throw symbol_not_found(head()->car.get());
    }
}

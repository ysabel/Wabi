#include "Parser.h"
#include "Sexp.h"


#include <deque>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;
using namespace Wabi;

ostream &operator<<(ostream &stream, const unique_ptr<Sexp> &sexp) {
    sexp->print(stream);
    return stream;
}

template <typename InputIt, typename OutputIt>
InputIt readNextSexp(InputIt first, InputIt last, OutputIt destination) {
    if (last == first) { return last; }
    auto l = first + 1;
    if (first->isStart()) {
        // Look for balanced parens
        size_t depth = 1;
        while (0 < depth && l != last) {
            if (l->isStart()) { ++depth; }
            else if (l->isEnd()) { --depth; }
            ++l;
        }
        if (0 < depth && l == last) { throw syntax_error("Unbalanced parens"); }
    }
    *destination++ = unique_ptr<Sexp>(readFromTokens(first, l));
    return l;
}

template <typename Tokens, typename InputIt, typename OutputIt>
InputIt readline(ostream &output, istream &input, Tokens &tokens, InputIt first, OutputIt destination) {
    string line;
    bool ok = getline(input, line);
    if (!ok) { return tokens.end(); }
    tokenize(line.length(), line.c_str(), back_inserter(tokens));
    auto next = first;
    while (next != tokens.end()) {
        try {
            next = readNextSexp(next, tokens.end(), destination);
        } catch (parentheses_underflow) {
            output << " . ";
            next = readline(output, input, tokens, next, destination);
        }
    }
    return next;
}

template <typename OutputIt>
void read(ostream &output, ostream &error, istream &input, OutputIt destination) {
    output << "\n> ";
    deque<Token> tokens;
    try {
        readline(output, input, tokens, tokens.begin(), destination);
    } catch (syntax_error &se) {
        error << se.what() << endl;
    } catch (parentheses_overflow &po) {
        error << po.what() << endl;
    }
}

int main(int argc, char *argv[]) {
    // Make sure we show decimals on floating point output
    cout << showpoint;

    while (cin) {
        deque<unique_ptr<Sexp>> sexps;
        read(cout, cerr, cin, back_inserter(sexps));

        for (auto sexp = sexps.begin(); sexp != sexps.end(); ++sexp) {
            cout << (*sexp) << ": '" << (*sexp)->eval() << endl;
        }
    }
    return 0;
}

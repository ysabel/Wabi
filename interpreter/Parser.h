#pragma once
#include <cstddef>

namespace Wabi {
    class Token {
    public:
        Token(size_t length, const char *text) : length(length), text(text) {}
        size_t length;
        const char *text;

        bool isStart() const { return 1 == length && '(' == *text; }
        bool isEnd() const { return 1 == length && ')' == *text; }
        bool isQuote() const { return 1 == length && '\'' == *text; }
    };

    template <typename OutputIt>
    OutputIt tokenize(size_t length, const char *input, OutputIt destination) {
        const char *start = nullptr;
        size_t location = 0;
        while (location < length) {
            switch(input[location]) {
            case '(':
            case ')':
            case '\'':
                if (nullptr != start) {
                    *destination++ = Token(input + location - start, start);
                }
                start = nullptr;
                *destination++ = Token(1, input + location);
                break;
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
            case ' ':
                if (nullptr != start) {
                    *destination++ = Token(input + location - start, start);
                }
                start = nullptr;
                break;
            default:
                if (nullptr == start) { start = input + location; }
                break;
            }
            ++location;
        }
        if (nullptr != start) {
            *destination++ = Token(input + location - start, start);
        }
        return destination;
    }
}

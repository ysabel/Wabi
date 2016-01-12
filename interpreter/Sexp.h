#pragma once

#include <cctype>
#include <cstdlib>
#include <stdexcept>

// For the moment
#include <iostream>
#include <memory>
#include <string>

#include "Environment.h"
#include "Parser.h"

namespace Wabi {
    class syntax_error : public std::runtime_error {
    public:
        explicit syntax_error(const std::string& what) : std::runtime_error(what) {}
        explicit syntax_error(const char *what) : std::runtime_error(what) {}
    };

    class parentheses_underflow : public std::underflow_error {
    public:
        explicit parentheses_underflow(const std::string& what) : std::underflow_error(what) {}
        explicit parentheses_underflow(const char *what) : std::underflow_error(what) {}
    };
    
    class parentheses_overflow : public std::overflow_error {
    public:
        explicit parentheses_overflow(const std::string& what) : std::overflow_error(what) {}
        explicit parentheses_overflow(const char *what) : std::overflow_error(what) {}
    };

    class Cons;
    
    class Sexp {
    public:
        virtual ~Sexp() {}
        virtual std::unique_ptr<Sexp> eval(Environment& environment = Global) const {
            return std::unique_ptr<Sexp>(copy());
        }
        virtual void bind(const Cons *arguments, Environment& environment = Global) {
            throw std::invalid_argument("Cannot bind arguments to a non-function.");
        }

        virtual operator std::string() const { return ""; }
        virtual Sexp* copy() const { return new Sexp(*this); }
        virtual void print(std::ostream &stream) const { stream << (std::string)(*this); }
        virtual void dump(std::ostream &stream) const {
            stream << "(Sexp)";
        }
    };
    
    class Atom : public Sexp {
    public:
        virtual Sexp* copy() const { return new Atom(*this); }
        virtual void dump(std::ostream &stream) const {
            stream << "(Atom";
            Sexp::dump(stream);
            stream << ")";
        }
    };
    class Integer : public Atom {
    public:
        Integer(long value) : value(value) {}
        long value;
        
        virtual operator std::string() const { return std::to_string(value); }
        virtual Sexp* copy() const { return new Integer(*this); }
        virtual void dump(std::ostream &stream) const {
            stream << "(Integer";
            Atom::dump(stream);
            stream << ": " << value << ")";
        }
    };
    class Float : public Atom {
    public:
        Float(float value) : value(value) {}
        float value;
        
        virtual operator std::string() const { return std::to_string(value); }
        virtual Sexp* copy() const { return new Float(*this); }
        virtual void dump(std::ostream &stream) const {
            stream << "(Float";
            Atom::dump(stream);
            stream << ": " << value << ")";
        }
    };
    class Symbol : public Atom {
    public:
        Symbol(size_t length, const char *text) : value(text, length) {}
        // For the moment, anyway
        std::string value;
        
        virtual operator std::string() const { return value; }
        virtual Sexp* copy() const { return new Symbol(*this); }
        virtual void dump(std::ostream &stream) const {
            stream << "(Symbol";
            Atom::dump(stream);
            stream << ": '" << value << "')";
        }
    };

    class Cons : public Sexp {
    public:
        Cons(Sexp *car = nullptr, Sexp *cdr = nullptr)
            : car(std::unique_ptr<Sexp>(car)),
              cdr(std::unique_ptr<Sexp>(cdr)) {}
        Cons(std::unique_ptr<Sexp>&& car)
            : car(std::move(car)),
              cdr(std::unique_ptr<Sexp>(nullptr)) {}
        Cons(std::unique_ptr<Sexp>&& car,
             std::unique_ptr<Sexp>&& cdr)
            : car(std::move(car)),
              cdr(std::move(cdr)) {}
        std::unique_ptr<Sexp> car;
        std::unique_ptr<Sexp> cdr;

        virtual Sexp* copy() const {
            return new Cons(nullptr == car.get() ? nullptr : car->copy(),
                            nullptr == cdr.get() ? nullptr : cdr->copy());
        }
        virtual void print(std::ostream &stream) const {
            stream << "(";
            if (nullptr != car.get()) { car->print(stream); }
            else { stream << "nil"; }
            stream << " . ";
            if (nullptr != cdr.get()) { cdr->print(stream); }
            else { stream << "nil"; }
            stream << ")";
        }
        virtual void dump(std::ostream &stream) const {
            stream << "(Cons";
            Sexp::dump(stream);
            stream << ": [ ";
            car->dump(stream);
            stream << " . ";
            cdr->dump(stream);
            stream << "])";
        }
    };
    
    class List : public Sexp {
    public:
        List() : head_(std::unique_ptr<Cons>(nullptr)), tail_(nullptr) {}
        Cons* head() { return head_.get(); }
        const Cons* head() const { return head_.get(); }
        Cons* tail() { return tail_; }
        const Cons* tail() const { return tail_; }

        void push(std::unique_ptr<Sexp>&& sexp) {
            if (nullptr == tail_) {
                head_ = std::make_unique<Cons>(std::move(sexp));
                tail_ = head();
            } else {
                Cons *newtail = new Cons(std::move(sexp));
                tail_->cdr = std::unique_ptr<Cons>(newtail);
                tail_ = newtail;
            }
        }
        void push(Sexp *sexp) {
            if (nullptr == tail_) {
                head_ = std::make_unique<Cons>(sexp);
                tail_ = head();
            } else {
                Cons *newtail = new Cons(sexp);
                tail_->cdr = std::unique_ptr<Cons>(newtail);
                tail_ = newtail;
            }
        }
        size_t length() const {
            size_t length = 0;
            for (auto sexp = head(); sexp != nullptr; sexp = dynamic_cast<const Cons *>(sexp->cdr.get())) {
                ++length;
            }
            return length;
        }
        

        virtual std::unique_ptr<Sexp> eval(Environment& environment = Global) const;

        virtual Sexp* copy() const {
            throw std::logic_error("List::copy currently unimplemented.");
        }
        virtual void print(std::ostream &stream) const {
            stream << "(";
            for (auto sexp = head(); sexp != nullptr; sexp = dynamic_cast<Cons *>(sexp->cdr.get())) {
                sexp->car->print(stream);
                if (nullptr != sexp->cdr.get()) {
                    stream << " ";
                }
            }
            stream << ")";
        }
        virtual void dump(std::ostream &stream) const {
            stream << "(List";
            Sexp::dump(stream);
            stream << ": [ ";
            for (auto sexp = head(); sexp != nullptr; sexp = dynamic_cast<Cons *>(sexp->cdr.get())) {
                sexp->car->dump(stream);
                stream << " ";
            }
            stream << "])";
        }
    private:
        std::unique_ptr<Cons> head_;
        Cons *tail_;
    };

    Atom *makeAtom(const Token &token);
    
    template <typename InputIt>
    Sexp *readFromTokens(InputIt first, InputIt last) {
        // Handle individual atoms
        if (1 == std::distance(first, last) && !first->isStart()) {
            if (first->isEnd()) {
                throw parentheses_overflow("Extra close parentheses at end of input.");
            }
            return makeAtom(*first);
        }

        // Anything else is a list or cons cell.
        List *sexps = new List();
        // If we start with a start token and end with an end token,
        // don't make a top level list with nothing in it but a list.
        if (first->isStart()) {
            if (last != first && (last - 1)->isEnd()) { --last; }
            ++first;
        }
        for (auto token = first; token != last; ++token) {
            if (token->isStart()) {
                size_t depth = 1;
                auto end = token + 1;
                while (0 < depth && end != last) {
                    if (end->isStart()) { ++depth; }
                    else if (end->isEnd()) { --depth; }
                    ++end;
                }
                if (0 < depth && end == last) {
                    throw parentheses_underflow("Unbalanced parentheses (" +
                                                std::to_string(depth) +
                                                "), get more tokens.");
                }
                sexps->push(readFromTokens(token + 1, end - 1));
                token = end - 1;
                if (token == last) { break; }
            } else if (token->isEnd()) {
                continue;
            } else {
                sexps->push(makeAtom(*token));
            }
        }
        // If it looks like a Cons cell instead of a list (i.e. two
        // Sexps separated by a '.' symbol) then return a plain Cons
        // cell instead of a List.
        if (3 == sexps->length()) {
            Cons *second = dynamic_cast<Cons *>(sexps->head()->cdr.get());
            Symbol *symbol = dynamic_cast<Symbol *>(second->car.get());
            if (nullptr != symbol && "." == symbol->value) {
                Cons *cons = new Cons(std::move(sexps->head()->car),
                                      std::move(sexps->tail()->car));
                delete sexps;
                return cons;
            }
        }

        return sexps;
    }

    class symbol_not_found : public std::invalid_argument {
    public:
        explicit symbol_not_found(Sexp *symbol)
            : std::invalid_argument("Symbol '" + std::string(*symbol) + "' not found.") {}
    };

}

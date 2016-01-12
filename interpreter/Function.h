#pragma once

#include <functional>
#include <memory>

#include "Sexp.h"

namespace Wabi {

    class Builtin : public Sexp {
    public:
        Builtin(std::function<Sexp *(const List *, Environment&)> implementation) : implementation(implementation) {}
        
        virtual Sexp* copy() const { return new Builtin(implementation); }
        virtual std::unique_ptr<Sexp> eval(Environment& environment = Global) const;
        virtual void bind(const Cons *arguments, Environment& environment = Global);
    private:
        std::function<Sexp *(const List *, Environment&)> implementation;
        std::unique_ptr<List> evaluatedArguments;
    };

}

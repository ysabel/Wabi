#include "Environment.h"
#include "Function.h"

using namespace std;
using namespace Wabi;

unique_ptr<Sexp> Builtin::eval(Environment& environment) const {
    return unique_ptr<Sexp>(implementation(evaluatedArguments.get(), environment));
}

void Builtin::bind(const Cons *arguments, Environment& environment) {
    evaluatedArguments = make_unique<List>();
    for (auto sexp = arguments; sexp != nullptr; sexp = dynamic_cast<const Cons *>(sexp->cdr.get())) {
        evaluatedArguments->push(move(sexp->car.get()->eval(environment)));
    }
}

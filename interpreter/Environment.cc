#include "Environment.h"
#include "Function.h"
#include "Sexp.h"

using namespace std;

namespace Wabi {
    Environment Global;

    void PopulateStandardEnvironment(Environment& environment) {
        environment["+"] = make_unique<Builtin>([](const List *args, Environment&) {
                // Augh.
                Integer *result = new Integer(0);
                for (auto sexp = args->head(); sexp != nullptr;
                     sexp = dynamic_cast<const Cons *>(sexp->cdr.get())) {
                    const Integer *next = dynamic_cast<const Integer *>(sexp->car.get());
                    if (nullptr != next) { result->value += next->value; }
                }
                return result;
            });
    }
}

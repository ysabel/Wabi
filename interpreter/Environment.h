#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace Wabi {
    class Sexp;
    
    typedef std::map<std::string, std::unique_ptr<Sexp>> Environment;

    extern Environment Global;

    void PopulateStandardEnvironment(Environment& environment = Global);
}

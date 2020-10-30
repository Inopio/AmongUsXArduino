#define BS_implementation
#include "BS.hpp"

//std=c++17

BuildSelf::Build build(BuildSelf::Flags flags) noexcept {
    using namespace BuildSelf;

    Build b = Build::get_default(flags);
    b.name = "whatsMyRole";

    b.add_source("../src/Main.cpp");

    return b;
}

#include "Ease.hpp"

//std=c++17

Build build(Flags flags) noexcept {
   

    Build b = Build::get_default(flags);
    b.name = "whatsMyRole";

    b.add_source_recursively("../src/");

    b.add_library("gdi32");
    b.add_library("user32");

    return b;
}

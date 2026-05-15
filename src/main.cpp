#include "core/driver/compile.hpp"
#include "core/driver/cli.hpp"

int main(int argc, const char** argv)
{
    CompilerContext ctx = parse_command(argc, argv);

    Compiler{ctx}.compile_modules();
}

// fix printing race condition
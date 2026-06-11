#include <chrono>

#include "driver/compile.hpp"
#include "driver/cli.hpp"

using namespace std::chrono;

int main(int argc, const char** argv)
{
    CompilerContext ctx = parse_command(argc, argv);
    Compiler{ctx}.compile_modules();
}

// fix printing race condition
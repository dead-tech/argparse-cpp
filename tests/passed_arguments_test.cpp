#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Passed arguments test", "[argparse]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "first-arg", "second-arg" };
    const auto  parser = argparse::ArgumentParser(argc, argv);

    REQUIRE(parser.args() == std::vector<std::string>{ "program_name", "first-arg", "second-arg" });
}

#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Require at least one long option for positional argument", "[argparse][require-long-opt]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };

    auto parser = argparse::ArgumentParser(argc, argv);

    REQUIRE_THROWS(parser.add_argument("-H"));
}

TEST_CASE("Duplicate argument", "[argparse][duplicate-arguments]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--foo");
    REQUIRE_THROWS(parser.add_argument("--foo"));
}

TEST_CASE("Argument conflicts with builtin", "[argparse][builtin-conflict]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };

    auto parser = argparse::ArgumentParser(argc, argv);

    REQUIRE_THROWS(parser.add_argument("--version"));
    REQUIRE_THROWS(parser.add_argument("-V"));
    REQUIRE_THROWS(parser.add_argument("--help"));
    REQUIRE_THROWS(parser.add_argument("-H"));
}

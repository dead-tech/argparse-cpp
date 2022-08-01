#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Count occurence of optional argument", "[argparse][count][debug]")
{
    const int   argc   = 3;
    const char *argv[] = { "program", "--verbose", "--verbose" };

    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("--verbose").count();

    const auto args = parser.parse_args();

    REQUIRE(args.at("--verbose").as<int>() == 2);
}

#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Metavar test", "[argparse][metavar]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--foo").set_metavar("TEST");

    const auto args = parser.parse_args();

    REQUIRE(parser.get_help_message() == "usage: program_name [-H] [--foo TEST] \n\nrequired arguments:\n\noptional arguments:\n  -H, --help\t\tshow this help message and exit\n  --foo TEST \n");
    REQUIRE(parser.get_usage_message() == "usage: program_name [-H] [--foo TEST] \n\n");
};

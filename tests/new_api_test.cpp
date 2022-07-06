#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("new api test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--quiet" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    const auto arg = parser.add_argument("--quiet").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
}

#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>
#include <iostream>


TEST_CASE("Required arguments test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--fail" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--fail", argparse::ArgTypes::BOOL, true);

    const auto args = parser.parse_args();

    REQUIRE(args.at("--fail").as<bool>() == true);
}

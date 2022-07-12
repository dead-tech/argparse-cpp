#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>
#include <iostream>


TEST_CASE("Required arguments test", "[argparse][required]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--fail" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--fail").set_type(argparse::ArgTypes::BOOL).set_flags(argparse::ArgFlags::REQUIRED);

    const auto args = parser.parse_args();

    REQUIRE(args.at("--fail").as<bool>() == true);
}

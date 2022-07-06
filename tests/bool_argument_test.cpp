#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Bool type arguments test (store true)", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--quiet" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    const auto arg = parser.add_argument("--quiet").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
}

TEST_CASE("Bool type arguments test (store false)", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--run" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    const auto arg = parser.add_argument("--run")
                       .set_type(argparse::ArgTypes::BOOL)
                       .set_flags(argparse::ArgFlags::STORE_FALSE)
                       .set_help("run the program");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--run").as<bool>() == false);
}

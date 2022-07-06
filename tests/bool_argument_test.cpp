#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Bool type arguments test (store true)", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--quiet" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--quiet", argparse::ArgTypes::BOOL, argparse::ArgFlags::STORE_TRUE, "quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
}

TEST_CASE("Bool type arguments test (store false)", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--run" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--run", argparse::ArgTypes::BOOL, argparse::ArgFlags::STORE_FALSE, "run the program");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--run").as<bool>() == false);
}

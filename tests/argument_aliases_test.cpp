#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Argument aliases test", "[argparse][aliases]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "--quiet", "-M" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--quiet", "-Q", "--not-verbose").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");
    parser.add_argument("--mode", "-M").set_type(argparse::ArgTypes::BOOL).set_help("display program version and exit");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
    REQUIRE(args.at("--mode").as<bool>() == true);
}

TEST_CASE("Argument aliases test (not present)", "[argparse][aliases]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--quiet", "-Q", "--not-verbose").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == false);
}

TEST_CASE("Argument aliases test (short opt)", "[argparse][aliases]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "-Q" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--quiet", "-Q", "--not-verbose").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
}

TEST_CASE("Argument aliases test (long opt)", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--not-verbose" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--quiet", "-Q", "--not-verbose").set_type(argparse::ArgTypes::BOOL).set_help("quiet mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--quiet").as<bool>() == true);
}

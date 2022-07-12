#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Default value test (used)", "[argparse][default]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name" };

    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("--file-path").set_default("default_value.txt");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--file-path").as<std::string>() == "default_value.txt");
}

TEST_CASE("Default value test (not used)", "[argparse][default]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "--file-path", "test.txt" };

    argparse::ArgumentParser parser(argc, argv);
    parser.add_argument("--file-path").set_default("default_value.txt");

    const auto args = parser.parse_args();

    REQUIRE(args.at("--file-path").as<std::string>() == "test.txt");
}

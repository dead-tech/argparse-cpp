#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Help message test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--help" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--build-dir", argparse::ArgTypes::STRING, true, "path to build directory");
    parser.add_argument("--repeat", argparse::ArgTypes::BOOL);

    const auto args = parser.parse_args();

    REQUIRE(parser.get_help_message() == "usage: program_name [--help] [--repeat] --build-dir \n\noptional arguments:\n  --help\t\tshow this help message and exit\n  [--repeat] --REPEAT \n\n\nrequired arguments:\n  --build-dir --BUILD-DIR path to build directory\n");
}

TEST_CASE("Usage message test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--help" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--build-dir", argparse::ArgTypes::STRING, true, "path to build directory");
    parser.add_argument("--repeat", argparse::ArgTypes::BOOL);

    const auto args = parser.parse_args();

    REQUIRE(parser.get_usage_message() == "usage: program_name [--help] [--repeat] --build-dir \n\n");
}

#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Help message test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--help" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--build-dir")
      .set_type(argparse::ArgTypes::STRING)
      .set_flags(argparse::ArgFlags::REQUIRED)
      .set_help("path to build directory");

    parser.add_argument("--repeat", "-R").set_type(argparse::ArgTypes::BOOL);

    const auto args = parser.parse_args();

    REQUIRE(parser.get_help_message() == "usage: program_name [-H] [--repeat --REPEAT] --build-dir --BUILD-DIR \n\nrequired arguments:\n  --build-dir --BUILD-DIR path to build directory\n\noptional arguments:\n  -H, --help		show this help message and exit\n  --repeat, -R --REPEAT \n");
}

TEST_CASE("Usage message test", "[argparse]")
{
    const int   argc   = 2;
    const char *argv[] = { "program_name", "--help" };
    auto        parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--build-dir")
      .set_type(argparse::ArgTypes::STRING)
      .set_flags(argparse::ArgFlags::REQUIRED)
      .set_help("path to build directory");

    parser.add_argument("--repeat").set_type(argparse::ArgTypes::BOOL);

    const auto args = parser.parse_args();

    REQUIRE(parser.get_usage_message() == "usage: program_name [-H] [--repeat --REPEAT] --build-dir --BUILD-DIR \n\n");
}

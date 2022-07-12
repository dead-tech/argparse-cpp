#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Version test (specified)", "[argparse][version]")
{
    const int   argc   = 2;
    const char *argv[] = { "program", "--version" };

    auto parser = argparse::ArgumentParser(argc, argv, "1.0.0");

    const auto args = parser.parse_args();

    REQUIRE(parser.get_version() == "1.0.0");
}

TEST_CASE("Version test (not specified)", "[argparse][version]")
{
    const int   argc   = 2;
    const char *argv[] = { "program", "--version" };

    auto parser = argparse::ArgumentParser(argc, argv);

    const auto args = parser.parse_args();

    REQUIRE(parser.get_version() == "0.0.1");
}

#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Passed arguments test", "[argparse]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "first-arg", "second-arg" };
    const auto  parser = argparse::ArgumentParser<argc>(argv);

    REQUIRE(parser.args() == std::vector<std::string>{ "program_name", "first-arg", "second-arg" });
}

TEST_CASE("Verify added arguments end up in the map", "[argparse]")
{
    const int   argc   = 1;
    const char *argv[] = { "program_name", "first-arg", "second-arg" };
    auto        parser = argparse::ArgumentParser<argc>(argv);
    parser.add_argument("--verbose", argparse::ArgumentType::BOOL);
    parser.add_argument("--build-dir", argparse::ArgumentType::STRING);
    parser.add_argument("--size", argparse::ArgumentType::INT);

    REQUIRE(parser["--verbose"].as<bool>() == false);
    REQUIRE(parser["--build-dir"].as<std::string>().empty());
    REQUIRE(parser["--size"].as<int>() == 0);
}

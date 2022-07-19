#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Nargs (positional arguments)", "[argparse][nargs]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "files.txt", "bar.json" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file-path").set_nargs(2);

    const auto args = parser.parse_args();

    REQUIRE(args.at("file-path").as<std::vector<std::string>>().size() == 2);
    REQUIRE(args.at("file-path").as<std::vector<std::string>>() == std::vector<std::string>{ "files.txt", "bar.json" });
}

TEST_CASE("Nargs (optional arguments)", "[argparse][nargs]")
{
    const int   argc   = 6;
    const char *argv[] = { "program_name", "files.txt", "bar.json", "--dir", "build/", "out/" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file-path").set_nargs(2);
    parser.add_argument("--dir").set_nargs(2);

    const auto args = parser.parse_args();

    REQUIRE(args.at("file-path").as<std::vector<std::string>>().size() == 2);
    REQUIRE(args.at("--dir").as<std::vector<std::string>>() == std::vector<std::string>{ "build/", "out/" });
}

TEST_CASE("Nargs (optional in positional argument)", "[argparse][nargs]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "files.txt", "--fail" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file-path").set_nargs(2);
    parser.add_argument("--fail");

    REQUIRE_THROWS(parser.parse_args());
}

TEST_CASE("Nargs (not enough arguments)", "[argparse][nargs]")
{
    const int   argc   = 3;
    const char *argv[] = { "program_name", "files.txt", "fail" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file-path").set_nargs(3);

    REQUIRE_THROWS(parser.parse_args());
}

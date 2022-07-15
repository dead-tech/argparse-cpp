#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Positional Arguments (one)", "[argparse][positional]")
{
    const int   argc   = 2;
    const char *argv[] = { "positional_arguments_test", "test.txt" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file").set_help("The file to read");

    const auto args = parser.parse_args();

    REQUIRE(args.at("file").as<std::string>() == "test.txt");
}

TEST_CASE("Positional Arguments (more)", "[argparse][positional]")
{
    const int   argc   = 4;
    const char *argv[] = { "positional_arguments_test", "test.txt", "out.txt", "dir/" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file").set_help("The file to read");

    parser.add_argument("out-file").set_help("The file to write");

    parser.add_argument("out-dir").set_help("The output directory");

    const auto args = parser.parse_args();

    REQUIRE(args.at("file").as<std::string>() == "test.txt");
    REQUIRE(args.at("out-file").as<std::string>() == "out.txt");
    REQUIRE(args.at("out-dir").as<std::string>() == "dir/");
}

TEST_CASE("Positional Arguments (mixed)", "[argparse][positional]")
{
    const int   argc   = 3;
    const char *argv[] = { "positional_arguments_test", "test.txt", "--release" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file").set_help("The file to read");

    parser.add_argument("--release").set_type(argparse::ArgTypes::BOOL).set_help("Release mode");

    const auto args = parser.parse_args();

    REQUIRE(args.at("file").as<std::string>() == "test.txt");
    REQUIRE(args.at("--release").as<bool>() == true);
}

TEST_CASE("Positional Arguments (wrong number)", "[argparse][positional]")
{
    const int   argc   = 3;
    const char *argv[] = { "positional_arguments_test", "--release" };

    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("file").set_help("The file to read");

    parser.add_argument("--release").set_type(argparse::ArgTypes::BOOL).set_help("Release mode");

    REQUIRE_THROWS(parser.parse_args());
}

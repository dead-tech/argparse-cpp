#include "argparse/argparse.hpp"
#include <catch2/catch.hpp>


TEST_CASE("Parse arguments test", "[argparse]")
{
    const int   argc   = 6;
    const char *argv[] = {
        "program_name", "--build-dir", "build/", "--count", "3", "--repeat",
    };
    auto parser = argparse::ArgumentParser(argc, argv);

    parser.add_argument("--build-dir");
    parser.add_argument("--count", argparse::ArgTypes::INT);
    parser.add_argument("--repeat", argparse::ArgTypes::BOOL);


    const auto args = parser.parse_args();

    REQUIRE(args.at("--build-dir").as<std::string>() == "build/");
    REQUIRE(args.at("--count").as<int>() == 3);
    REQUIRE(args.at("--repeat").as<bool>() == true);
}

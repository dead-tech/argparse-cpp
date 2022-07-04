![Build Status](https://github.com/dead-tech/argparse-cpp/actions/workflows/linux.yml/badge.svg)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/dead-tech/argparse-cpp/main.svg)](https://results.pre-commit.ci/latest/github/dead-tech/argparse-cpp/main)
# argparse-cpp

Argument Parser inspired by python's argparse module.

## Example

Simple example program to print the content of the file at the given filepath

```cpp
#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, const char **argv)
{
    argparse::ArgumentParser parser(argc, argv);

    parser.add_argument("--file-path", argparse::ArgTypes::STRING, true);

    const auto args = parser.parse_args();

    const auto file_path = args.at("--file-path").as<std::string>();

    std::ifstream file(file_path);
    std::stringstream ss{};
    ss << file.rdbuf();

    std::cout << ss.str() << '\n';
}
```

## Quickstart

### Downloading the header file

- Get the file through wget
   ```console
   $ wget https://raw.githubusercontent.com/dead-tech/argparse-cpp/main/include/argparse/argparse.hpp
   ```
- Add the file into your include path
- Include the file in your source dode

### Cloning the repo

- Clone the repo
   ```console
   $ git clone https://github.com/dead-tech/argparse-cpp.git
   $ cd argparse-cpp
   ```
- Move the header file
   ```console
   $ mv include/argparse.hpp <your-project-include-path>
   ```
- Include the file in your source dode

## Testing

This project uses catch2 as a testing framework.

All the available tests can be found [here](tests/).

The following steps are what is required to run the test suite.

```console
$ git clone https://github.com/dead-tech/argparse-cpp.git
$ cd argparse-cpp
$ mkdir build && cd build
$ cmake .. && make
$ ./argparse-cpp_tests
```

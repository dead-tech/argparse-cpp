![Build Status](https://github.com/dead-tech/argparse-cpp/actions/workflows/linux.yml/badge.svg)
![Deploy Status](https://github.com/dead-tech/argparse-cpp/actions/workflows/deploy.yml/badge.svg)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/dead-tech/argparse-cpp/main.svg)](https://results.pre-commit.ci/latest/github/dead-tech/argparse-cpp/main)
# argparse-cpp

Argument Parser in C++ inspired by python's argparse module.

## Table of contents
   * [Basic Example](#example)
   * [Quickstart](#quickstart)
      * [Conan](#using-conan-package-manager)
      * [Source Code](#download-source-code-from-latest-release)
      * [Header File](#downloading-the-header-file)
      * [Clone Repo](#cloning-the-repo)
   * [Documentation](#documentation)
   * [Testing](#testing)
   * [License](#license)

## Example

Simple example program that emulates an hypothetically compiler CLI.

```cpp
#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, const char **argv)
{
    argparse::ArgumentParser parser(argc, argv);

   parser.add_argument("files")
         .set_type(argparse::ArgTypes::STRING)
         .set_default("test.txt")
         .set_help("Paths to the files to compile")
         .set_flags(argparse::ArgFlags::REQUIRED)
         .set_metavar("FILE_PATH")
         .set_nargs('+');

   parser.add_argument("--release", "-R")
         .set_type(argparse::ArgTypes::BOOL)
         .set_help("Build in release version");

   const auto args = parser.parse_args();

   const auto files = args.at("files").as<std::vector<std::string>>();
   const auto is_release = args.at("--release").as<bool>();

   const auto result = build_files(files, is_release);
}
```

## Quickstart

### Using conan package manager

**Starting from release [v0.1.0](https://github.com/dead-tech/argparse-cpp/releases/tag/v0.1.0) it is possible to use conan to install the header only library from the artifactory remote.**

- Create a simple conanfile.txt in the root of your project

   ```txt
   [requires]
   argparse-cpp/0.1.1@dead/stable

   [generators]
   cmake
   ```

- Move into directory where you want to build your project

   ```bash
   $ cd <build-dir>
   ```

- Add the artifactory remote to your conan profile

   ```bash
   $ conan remote add argparse-cpp https://argparsecpp.jfrog.io/artifactory/api/conan/argparse-cpp-conan-local
   ```

- Run conan install

   ```bash
   $ conan install .. -r argparse-cpp
   ```

### Download source code from latest release

Click on the releases header on the right side of the GitHub page or navigate [here](https://github.com/dead-tech/argparse-cpp/releases).

Scroll down to the assets section and download the source code (zip or tar.gz).
### Downloading the header file

- Get the file through wget
   ```bash
   $ wget https://raw.githubusercontent.com/dead-tech/argparse-cpp/main/include/argparse/argparse.hpp
   ```

### Cloning the repo

- Clone the repo
   ```bash
   $ git clone https://github.com/dead-tech/argparse-cpp.git
   $ cd argparse-cpp
   ```
- Move the header file
   ```bash
   $ mv include/argparse.hpp <your-project-include-path>
   ```

## Documentation

Documentation can be found [here](dead-tech.github.io/argparse-cpp).

## Testing

This project uses catch2 as a testing framework.

All the available tests can be found [here](tests/).

The following steps are what is required to run the test suite.

```bash
$ git clone https://github.com/dead-tech/argparse-cpp.git
$ cd argparse-cpp
$ mkdir build && cd build
$ conan install ..
$ cmake .. && make
$ ./argparse-cpp_tests
```

If these steps do not work properly for you see the [official docs](https://dead-tech.github.io/argparse-cpp/user_guide/running_tests.html).

## License

The project is licensed under MIT License (see [LICENSE](LICENSE)).

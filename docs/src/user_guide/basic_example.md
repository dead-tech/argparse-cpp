# Basic Example

Simple example program that emulates an hypothetically compiler CLI.
You may use this to get a general idea of what it would like to crete an application using this library.

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

After compiling the program can ben run at the command line as follows:

```bash
$ ./program_name args...
```

**Note**: You will get automatically generated **help** and **version** optional arguments that will respectively produce the following output

Version optional argument:
```bash
$ ./main --version
0.0.1
```

Help optional argument:
```bash
$ ./main --help
usage: ./main [-H] [--release --RELEASE] files FILE_PATH

required arguments:
  files FILE_PATH Paths to the files to compile

optional arguments:
  -H, --help            show this help message and exit
  --release, -R --RELEASE Build in release version
```

When run with the appropriate arguments it should print the following output:

```bash
$ ./main files.txt bar.json --release
Built files:
files.txt bar.json
```

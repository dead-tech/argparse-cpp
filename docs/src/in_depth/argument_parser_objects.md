# ArgumentParser objects

### Function signature

```cpp
class ArgumentParser
{
public:
    ArgumentParser(const int argc, const char **argv, std::string version = "0.0.1");
}
```

The constructor creates a new `ArgumentParser` object. All the parameters except for the version are required. Each of the parameters is described more in detail below.

- `argc` - The number of arguments passed to the program
- `argv` - The array of arguments passed to the program
- `version` - The version of the program (optional - defaults to "0.0.1")

### Detail

**argc**
This parameter is used to determine the number of arguments passed to the program. Not only in C++ there is no practical way of eliminating this parameter from the function signature, but this makes it also convenient to test the library.

**argv**
This parameter represent the array of arguments passed to the program. As for [argc](#argc) not only in C++ there is no practical way of eliminating this parameter from the function signature, but this makes it also convenient to test the library or perhaps providing your own arguments for whatever reason.

**version**
This parameter is used to provide a custom version of the program. If it is left blank it will default to "0.0.1". Note that the library will automatically generate an optional argument, as a builtin, for displaying the version of the program on command.
The builtin can be invoked with the `--version` or `-V` argument as shown below.
```bash
$ ./program_name --version
0.0.1
```

<br>

`ArgumentParser` class full [source code](https://github.com/dead-tech/argparse-cpp/blob/main/include/argparse/argparse.hpp#L300-L696).

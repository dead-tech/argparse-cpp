# set_help()

### Rationale

This method allows you to set a custom help message that has to be printed next to the argument name when invoking the `--help` builtin.

### Example usage

```cpp
parser.add_argument("--release", "-R").set_help("Set build process in release mode");
```

### Source Code

```cpp
Arg &set_help(const std::string &help_message)
{
    this->help_message = help_message;
    return *this;
}
```

# set_nargs()

### Rationale

This method allows you to set how many values are expected for an argument.

### Possible values

You may specify as an argument to this function one of the following values:
 - x -> any integer number
 - \* -> which means at zero or more values
 - \+ -> which means at least one or more values

### Example usage

```cpp
parser.add_argument("files").set_nargs('*');
```

### Source Code

```cpp
Arg &set_nargs(const auto nargs)
{
    static constexpr auto is_number = std::is_same_v<decltype(nargs), const int>;
    static constexpr auto is_symbol = std::is_same_v<decltype(nargs), const char>;

    if constexpr (is_number) {
        this->nargs = nargs;
    } else if constexpr (is_symbol) {
        this->nargs = nargs - '0';
    } else {
        throw exceptions::ArgparseException(
            std::source_location::current(),
            "set_nargs() error: unsupported type: %\nSupported types are: int, std::string\n",
            typeid(nargs).name());
    }

    return *this;
}
```

Moreover the function uses C++17 `if constexpr` to generate at compile time the right branch of the `if` statement, reducing the overhead during runtime.
<br>

**Note:** Be aware that the function will throw an exception if the argument is not a number or one of the supported symbols symbol.

# set_default()

### Rationale

This method allows you to set a default value for an argument. This value will be used if the user does not provide a value for the argument.

**NOTE**: Be aware that if a required argument is not provived the default value will be used and no error will be thrown.

**IMPORTANT:** Executing a builting command such as `--help` or `--version` will quit the program with zero exit code.

### Example usage

```cpp
parser.add_argument("--jobs", "-J").set_default(1);
```

### Source Code

```cpp
template<SupportedArgumentType T>
Arg &set_default(T &&value)
{
    if constexpr (std::is_convertible_v<T, std::string>) {
        this->values.front() = std::forward<T>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        this->values.front() = utils::bool_to_str(std::forward<T>(value));
    } else {
        this->values.front() = std::to_string(std::forward<T>(value));
    }

    return *this;
}
```

This templated function is restricted in its types by the concept `SupportedArgumentTypes`. The latter allows you to pass as an argument to the call just the types that are supported by the library (see [set_type()](set_type.md) for more info).
<br>
Moreover the function uses C++17 `if constexpr` to generate at compile time the right branch of the `if` statement, reducing the overhead during runtime.

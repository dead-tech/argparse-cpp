# The parse_args() method

### Function signature

```cpp
[[nodiscard]] map_type parse_args();
```

### Explanation of how it works

- `map_type` is equivalent to `std::unordered_map<std::string, Arg>`
- The function will immediately create the usage and help messages to be display on command
    ```cpp
    this->create_usage_message();
    this->create_help_message();
    ```
- The function will also before even parsing the command line arguments check if any builtins are present and if so, execute them
    ```cpp
    if (const auto builtin = this->get_builtin_if(); builtin.has_value()) {
        const auto fn = builtin.value();
        fn();
        exit(0);
    }
    ```
- The function will then split the program arguments into positional and optional arguments
    ```cpp
    const auto [positional_args, optional_args] = this->split_program_args();
    ```
- The function will then parse the positional arguments
    ```cpp
    this->parse_positional_args(positional_args);
    ```
- The function will then throw if a not registered optional argument is found in the program args
    ```cpp
    this->throw_if_unrecognized(optional_args);
    this->parse_optional_args(optional_args);
    ```

Whatever is returned from the function can be then accessed through the .at() method and then cast to the appropriate type as it is explained [here](./arg_methods/as.md).


### Exceptions
Be also aware that this function may throw an exception in one of these cases:
 - If not enough positional arguments were provided
 - If an optional argument that is not registered was provided

**NOTE**: The return type of the method is marked as `[[nodiscard]]` which means that the result of the call to this method cannot be ignored and the value has necessarily to be stored in a variable.

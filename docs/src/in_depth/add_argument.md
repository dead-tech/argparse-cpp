# The add_argument() method

### Function signature

```cpp
template<utils::StringLike... Names>
mapped_type &add_argument(Names &&...names);
```

- `utils::StringLike` - A C++20 concept that allows you to pass any type `T` which is convertible to a `std::string`

- `Names &&...names` - A variadic pack of `utils::StringLike`s containing all the names for the argument, including the primary name and the optional aliases

- `mapped_type &` - The return type of the method which is a reference to the argument object. By returning a non-`const` reference to the newly created argument object, you can modify the argument object by using the object chaining method, which our API is based on.
_mapped_type_ is just an aliased type:
    ```cpp
    using map_type       = std::unordered_map<std::string, Arg>;
    using mapped_type    = map_type::mapped_type;
    ```

### Constraints

**Important:** As shown by the `static_assert` below you must provide at least one argument as a name for the argument itself. If you're trying to register an optional argument one of the names **must** also start with a `--`. The latter name will be used as the primary name of the argument, for example to index and retrieve the argument in the internal storage. Whereas for positional arguments you should pass only one argument, the others will be discard and only the first one will be used.

```cpp
static_assert(
    sizeof...(Names) > 0,
    "[argparse] error: add_argument() needs at least one argument as a name (starting with '--' for "
    "positional arguments)");
```

### Runtime dispatch based on argument kind

As we have briefly mentioned above the same method can be used to create both optional and positional arguments. The system will automatically make its way between the two and dispatch to the appropriate method.

```cpp
if (arg_kind == ArgKind::Positional) {
    return this->add_positional_argument(data);
} else if (arg_kind == ArgKind::Optional) {
    return this->add_optional_argument(data, primary_name.value());
}
```

<br>

`add_argument()` method full [source code](https://github.com/dead-tech/argparse-cpp/blob/main/include/argparse/argparse.hpp#L317-L338).

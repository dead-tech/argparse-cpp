# as()

### Rationale

This method allows you to cast the `Arg` value (a.k.a. `std::string`) you get when invoking the `at()` method on the return value of the `parse_args()` method ([more info](../parse_args.md)) to the type of your needs.

### List of supported types
 - `int` - `std::is_integral_v<T>` - actually every integral type
 - `std::vector<int>`
 - `std::string`
 - `std::vector<std::string>`
 - `bool`

### Example usage

```cpp
const auto args = parser.parse_args();
const auto value = args.at('--value').as<int>();
```

### Source Code

```cpp
template<typename ReturnType>
[[nodiscard]] decltype(auto) as() const
{
    static constexpr auto is_int        = std::is_integral_v<ReturnType> && !std::is_same_v<ReturnType, bool>;
    static constexpr auto is_vec_int    = std::is_same_v<ReturnType, std::vector<int>>;
    static constexpr auto is_string     = std::is_same_v<ReturnType, std::string>;
    static constexpr auto is_vec_string = std::is_same_v<ReturnType, std::vector<std::string>>;
    static constexpr auto is_bool       = std::is_same_v<ReturnType, bool>;

    if constexpr (is_int) {
        return utils::impl::str_to_int_helper(this->values.front());
    } else if constexpr (is_vec_int) {
        return utils::str_to_int(this->values);
    } else if constexpr (is_string) {
        return values.front();
    } else if constexpr (is_vec_string) {
        return std::vector(values.data(), values.data() + this->actual_size);
    } else if constexpr (is_bool) {
        return utils::str_to_bool(this->values.front());
    }

    throw exceptions::ArgparseException(
        std::source_location::current(),
        "as<%>() error: unsupported return type\nSupported types are: int, std::vector<int>, std::string, "
        "std::vector<std::string>, bool\n",
        typeid(ReturnType).name());
}
```

The function uses C++17 `if constexpr` to generate at compile time the right branch of the `if` statement, reducing the overhead during runtime.

**NOTE**: If the type passed as a template argument to this method is not one of the supported types, the function will throw an exception.

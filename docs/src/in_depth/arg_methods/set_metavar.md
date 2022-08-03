# set_metavar()

### Rationale

This method allows you to specify a custom metavar. A metavar is the string it is printed next to the argument name when invoking the `--help` builtin that helps you understand where you have to specify the value of the argument.

Example: `program_name --out output_file`

In the example above <i>output_file</i> is the metavar.

### Example usage

```cpp
parser.add_argument("--out", "-O").set_metavar("output_file");
```

### Source Code

```cpp
Arg &set_metavar(const std::string &metavar)
{
    this->metavar = metavar;
    return *this;
}
```

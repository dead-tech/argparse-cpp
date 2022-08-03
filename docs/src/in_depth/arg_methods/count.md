# count()

### Rationale

This method allows you to count the occurence of an argument in the command line arguments.

### Example usage

```cpp
parser.add_argument("--verbose").count();
```

### Source Code

```cpp
Arg &count()
{
    this->count_occurence = true;
    this->type            = ArgTypes::BOOL;
    return *this;
}
```

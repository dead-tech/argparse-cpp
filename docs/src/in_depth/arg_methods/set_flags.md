# set_flags()

### Rationale

This method allows you to set some flags for the argument that has to be parsed from the command line arguments.
<br>

We currently use a bitmask to encode all the various flags to ease the usage.

### List of flags you may specify

It is worth be aware of that if you do not call `set_flags()`, on a `Arg` instance, the type of the argument will default to `argparse::ArgFlags::DEFAULT` which is only `STORE_TRUE` at the moment as shown below.

```cpp
enum class ArgFlags : int64_t
{
    NONE        = 0,
    REQUIRED    = (1LL << 1),
    STORE_TRUE  = (1LL << 2),
    STORE_FALSE = (1LL << 3),

    DEFAULT = STORE_TRUE,
};
```

### Example usage

```cpp
parser.add_argument("--release", "-R")
    .set_flags(argparse::ArgFlags::REQUIRED | argparse::ArgFlags::STORE_TRUE);
```

### Source Code

```cpp
Arg &set_flags(const ArgFlags &flags)
{
    this->flags = flags;
    return *this;
}
```

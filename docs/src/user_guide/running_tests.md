# Running the tests

This project uses [Catch2](https://github.com/catchorg/Catch2) testing framework.
You can find the whole test suite [here](https://github.com/dead-tech/argparse-cpp/tree/main/tests).

If you would like to, you can run the test suite yourself by following the instructions below.

1. Clone the repository.

    ```bash
    $ git clone https://github.com/dead-tech/argparse-cpp
    ```

2. Change directory to the repository root.

    ```bash
    $ cd argparse-cpp
    ```

3. Create a build folder and cd into it.

    ```bash
    $ mkdir build && cd build
    ```

4. Install the conan dependecies.

    ```bash
    $ conan install ..
    ```

    If this does not work for you checkout the [installation section](installation.md) for further instructions.

5. Configure cmake and compile the tests

    ```bash
    $ cmake .. && make
    ```

    You can also specify a number of jobs as an argument to the `make` command to speed up the compilation process like so `make -j5`.

6. Run the tests

    ```bash
    $ ./argparse-cpp_tests
    ```

<br>

Currently all the tests are expected to: ![Build Status](https://github.com/dead-tech/argparse-cpp/actions/workflows/linux.yml/badge.svg)

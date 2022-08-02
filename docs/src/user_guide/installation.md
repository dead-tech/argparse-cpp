# Installation

There are different ways of installing the library. Choose whatever you think is the best for you.

- [Installation](#installation)
  - [Using conan package manager](#using-conan-package-manager)
  - [Download source code from latest release](#download-source-code-from-latest-release)
  - [Downloading the header file](#downloading-the-header-file)
  - [Cloning the repo](#cloning-the-repo)

## Using conan package manager

_Starting from release **v0.1.0** it is possible to use conan in order to install the library from the artifactory remote_

For these steps to work you will need both [conan](https://docs.conan.io/en/latest/installation.html) and [CMake](https://cmake.org/install/). Check their respective installation guides for further information on how to get those tools.

1. Create a simple conanfile.txt in the root of your project containing the requirements

    ```txt
    [requires]
      argparse-cpp/0.1.0@dead/stable

    [generators]
      cmake
    ```

2. Move into the build folder

    ```bash
    $ cd <build-folder>
    ```

3. Add the artifactory remote to your conan client

    ```bash
    $ conan remote add argparse-cpp https://argparsecpp.jfrog.io/artifactory/api/conan/argparse-cpp-conan-local
    ```

4. Run `conan install`

    ```bash
    $ conan install .. -r argparse-cpp
    ```

5. Configure cmake to use the installed library from conan

    ```cmake
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(NO_OUTPUT_DIRS TARGETS)
    ...
    target_link_libraries(your_exe PUBLIC CONAN_PKG::argparse-cpp)
    ```

## Download source code from latest release

Click on the releases header on the right side of the GitHub repo or navigate [here](https://github.com/dead-tech/argparse-cpp/releases)

Scroll down to the assets section and download the source code (zip or tar.gz).

## Downloading the header file

Get the header file through wget.

```bash
$ wget https://raw.githubusercontent.com/dead-tech/argparse-cpp/main/include/argparse/argparse.hpp
```

## Cloning the repo

1. Clone the repo

    ```bash
    $ git clone https://github.com/dead-tech/argparse-cpp.git
    ```

2. Change current directory to the cloned folder

    ```bash
    $ cd argparse-cpp
    ```

3. Move the header file

    ```bash
    $ mv include/argparse.hpp <your-project-include-path>
    ```

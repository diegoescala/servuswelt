# Servuswelt Example Project

This example demonstrates how to use the servuswelt library in your project.

## Prerequisites

- CMake 3.10 or higher
- C++17 compiler
- cpprestsdk
- servuswelt library (installed)

## Building the Example

First, make sure you have built and installed the servuswelt library:

```bash
cd ../servuswelt-lib
mkdir -p build && cd build
cmake ..
make
sudo make install
```

Then, build the example:

```bash
mkdir -p build && cd build
cmake ..
make
```

## Running the Example

After building, you can run the example:

```bash
./servuswelt-example
```

The server will start on http://localhost:8080 and provide an API endpoint at `/api/bank/transactions/:id`.

## How It Works

This example demonstrates:
1. Finding the servuswelt package using CMake's `find_package()`
2. Linking against the installed library with `target_link_libraries()`
3. Including the servuswelt headers using angle brackets: `#include <servuswelt/...>` 
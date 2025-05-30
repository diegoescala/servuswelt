# Servuswelt Example Project

This example demonstrates the elegant simplicity of the servuswelt library in action, showing how to create a small REST API server with minimal effort.

## Prerequisites

To build and run this example, you'll need:

- CMake 3.10 or higher
- C++17 compiler
- cpprestsdk library
- servuswelt library (properly installed)

## Building the Example

First, ensure the servuswelt library is built and installed:

```bash
# Install the servuswelt library
cd ../servuswelt-lib
mkdir -p build && cd build
cmake ..
make
sudo make install
```

Then build the example:

```bash
# Build the example project
cd ../../example
mkdir -p build && cd build
cmake ..
make
```

## Running the Example

Launch the server with:

```bash
./servuswelt-example
```

The server will start on http://localhost:8080, providing an API endpoint at `/api/bank/transactions/:id` that demonstrates parameter handling and JSON responses.

## Testing the API

Once the server is running, you can access the transaction endpoint by navigating to:

```
http://localhost:8080/api/bank/transactions/{id}
```

Replace `{id}` with any transaction identifier you wish to query. For example:

```
http://localhost:8080/api/bank/transactions/12345
```

### Expected Response

The server will return a JSON response with the transaction ID:

```json
{
  "id": "12345"
}
```

You can also use curl to test the endpoint:

```bash
curl http://localhost:8080/api/bank/transactions/12345
```

The API automatically handles CORS headers, making it ready for use with web applications.

## How It Works

This example showcases three key aspects of using servuswelt:

1. **Finding the Library** - Using CMake's `find_package(servuswelt)` to locate the installed library.

2. **Linking** - Connecting to the library via `target_link_libraries(${PROJECT_NAME} servuswelt::servuswelt)`.

3. **Including Headers** - Using the clean syntax of `#include <servuswelt/...>` for access to the API.

The code structure demonstrates how to organize routes using the Module pattern, separating concerns and creating a maintainable API structure.

---

This example serves as both documentation and inspiration - a starting point for your own journey with servuswelt. 
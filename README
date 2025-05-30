# Servuswelt

## The Philosophy of Simplicity in Routing

In the ever-evolving landscape of web service architecture, simplicity is not merely a feature—it is the foundation upon which robust systems are built. Servuswelt embodies this principle by distilling the complexity of HTTP routing into an elegant, intuitive interface that speaks the language of modern C++ development.

The name "Servuswelt" itself is a delightful play on words—"servus" being a versatile greeting meaning both "hello" and "goodbye," while "welt" means "world." Thus, "Servuswelt" is not only a nod to the timeless "Hello World" tradition of programming, but also embodies our philosophy: to serve the world through a harmonious balance of power and simplicity, while greeting and bidding farewell to requests with elegant efficiency.

## The Essence of Servuswelt

At its core, Servuswelt is a lightweight C++ routing library that embraces the expressive power of modern C++ while providing a clean, declarative API for defining RESTful endpoints. It is not merely a collection of functions; it is a canvas upon which developers paint their service architecture, with each route a deliberate brushstroke in the greater composition.

Servuswelt stands on the shoulders of the robust cpprestsdk, extending its capabilities with an interface that feels natural and unobtrusive. Like a well-designed musical instrument, it disappears in the hands of its user, allowing the developer to focus on the melody of their business logic rather than the mechanics of HTTP handling.

## The Art of the Route

```cpp
// The beauty of a route lies in its clarity
Router router("http://localhost:8080");

// A single line that encapsulates an entire endpoint
router.addRoute(Route(methods::GET, "/api/users/:id", 
    [](http_request request) {
        // Your symphony of logic here
    }));

// The elegance of dispatch
router.startDispatch();
```

In these few lines, we witness the convergence of form and function. The route declaration reads like a statement of intent—clear, concise, and purposeful. The parameter notation (`:id`) echoes the natural language of API design, while the lambda captures the essence of the handler with minimal ceremony.

## Installation: The Path to Enlightenment

```bash
# Clone the repository
git clone https://github.com/diegoescala/servuswelt.git

# Create a build directory
mkdir -p servuswelt-lib/build && cd servuswelt-lib/build

# Configure with CMake
cmake ..

# Build
make

# Install
sudo make install
```

## Integration: The Harmony of Components

```cmake
# Find the servuswelt package
find_package(servuswelt REQUIRED)

# Link against it
target_link_libraries(your_project servuswelt::servuswelt)
```

With these simple steps, your project becomes one with servuswelt, inheriting its elegance and power. The namespaced target ensures clean integration without pollution—a testament to our commitment to harmonious coexistence within the greater ecosystem.

## Example Project: A Guide for the Journey

To illuminate the path for new travelers, we provide an example project within the repository. This practical demonstration resides in the `example/` directory and showcases the library's capabilities through a simple yet insightful implementation.

The example project includes:
- A complete CMake configuration showing how to find and link against servuswelt
- A demonstration of creating routes and handling HTTP requests
- Practical patterns for organizing your code with modules

Since the example uses `find_package(servuswelt)`, you must first build and install the library before building the example:

```bash
# First, install the servuswelt library
cd servuswelt-lib
mkdir -p build && cd build
cmake ..
make
sudo make install

# Then build and run the example
cd ../../example
mkdir -p build && cd build
cmake ..
make
./servuswelt-example
```

This example serves not merely as documentation, but as a living companion on your journey with servuswelt—a constellation by which to navigate the possibilities of the library.

## Usage: The Dance of HTTP and C++

```cpp
#include <servuswelt/router.h>
#include <servuswelt/module.h>
#include <servuswelt/route.h>

class UserModule : public Module {
public:
    const std::vector<Route> getRoutes() const override {
        return {
            Route(methods::GET, "/users", std::bind(&UserModule::listUsers, this, std::placeholders::_1)),
            Route(methods::GET, "/users/:id", std::bind(&UserModule::getUser, this, std::placeholders::_1)),
            Route(methods::POST, "/users", std::bind(&UserModule::createUser, this, std::placeholders::_1))
        };
    }

private:
    void listUsers(http_request request) const {
        // Return all users
        web::json::value response = web::json::value::array();
        Router::replyWithCors(request, status_codes::OK, response);
    }

    void getUser(http_request request) const {
        // Extract the ID from the path
        auto path = request.relative_uri().path();
        auto id = path.substr(path.find_last_of('/') + 1);
        
        // Return the user
        web::json::value response;
        response[U("id")] = web::json::value::string(id);
        Router::replyWithCors(request, status_codes::OK, response);
    }

    void createUser(http_request request) const {
        // Create a new user
        Router::replyWithCors(request, status_codes::Created);
    }
};

int main() {
    Router router("http://localhost:8080");
    
    UserModule userModule;
    for (auto route : userModule.getRoutes()) {
        router.addRoute(route);
    }
    
    router.startDispatch();
    return 0;
}
```

In this orchestration of code, we see the true beauty of servuswelt—the separation of concerns, the clarity of intent, and the fluidity of execution. The Module abstraction provides a natural organizational unit, while the Router conducts the symphony of requests to their appropriate handlers.

## Conclusion: The Journey Continues

Servuswelt is more than a library; it is a philosophy of web service development that values clarity, simplicity, and expressiveness. In a world of increasing complexity, it stands as a reminder that elegant solutions need not be complicated.

We invite you to explore the world of servuswelt, to build upon its foundations, and to join us in the pursuit of beautiful, maintainable code.

---

*"Simplicity is the ultimate sophistication." — Leonardo da Vinci*

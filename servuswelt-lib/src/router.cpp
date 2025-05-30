#include "router.h"

#include <unordered_map>
#include <thread>
#include <chrono>

using namespace web::http;
using namespace web::http::experimental::listener;

namespace servuswelt {
    Router::Router(const std::string& base_uri):
        base_uri_(base_uri), shutdown_requested_(false) {}

    Router::~Router() {
        std::cout << "Router destructing, closing listeners..." << std::endl;
        
        // Create a vector of close tasks
        std::vector<pplx::task<void>> close_tasks;
        
        // Close all listeners
        for (auto& [path, listener] : listeners_) {
            if (listener != nullptr) {
                close_tasks.push_back(listener->close());
            }
        }
        
        // Wait for all listeners to close
        try {
            pplx::when_all(close_tasks.begin(), close_tasks.end()).wait();
            std::cout << "All listeners closed successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error during listener cleanup: " << e.what() << std::endl;
        }
    }

    void Router::addModule(std::shared_ptr<Module> module) {
        for (auto& route : module->getRoutes()) {
            addRoute(route);
        }
    }

    void Router::addRoute(Route route) {
        routes_.push_back(route);

        // Check if we already have a listener for this path
        auto path = route.getPath();
        // Remove the parameter part for the listener path
        auto base_path = path.substr(0, path.find("/:"));
        if (base_path.empty()) {
            base_path = path;
        }
        
        auto it = listeners_.find(base_path);
        
        if (it == listeners_.end()) {
            // Create new listener for this path
            auto listener = std::make_shared<http_listener>(base_uri_ + base_path);
            listeners_[base_path] = listener;

            // Add CORS handling for OPTIONS requests
            listener->support(methods::OPTIONS, [this](http_request request) {
                Router::replyWithCors(request, status_codes::OK);
            });

            // Add the method support to the listener with CORS headers
            listener->support(route.getMethod(), [this, route](http_request request) {
                addCorsHeaders(request);
                route.getHandler()(request);
            });

            open_tasks_.push_back(listener->open());
            std::cout << "Listening for requests at " << listener->uri().to_string() << std::endl;
        } else {
            // Add CORS handling for OPTIONS requests
            it->second->support(methods::OPTIONS, [this](http_request request) {
                Router::replyWithCors(request, status_codes::OK);
            });

            // Add the method support to the listener with CORS headers
            it->second->support(route.getMethod(), [this, route](http_request request) {
                addCorsHeaders(request);
                route.getHandler()(request);
            });
        }
    }

    void Router::startDispatch() {
        if (open_tasks_.empty()) {
            std::cerr << "No routes registered!" << std::endl;
            return;
        }

        std::cout << "Server started." << std::endl;
        
        try {
            // Wait for all listeners to be ready
            pplx::when_all(open_tasks_.begin(), open_tasks_.end()).wait();
            
            // Wait for shutdown signal
            std::unique_lock<std::mutex> lock(shutdown_mutex_);
            shutdown_cv_.wait(lock, [this]() { return shutdown_requested_; });
            
        } catch (const std::exception& e) {
            std::cerr << "Error in dispatch: " << e.what() << std::endl;
            throw;
        }
    }

    void Router::stop() {
        std::cout << "Stopping router..." << std::endl;
        
        // Signal shutdown
        {
            std::lock_guard<std::mutex> lock(shutdown_mutex_);
            shutdown_requested_ = true;
            shutdown_cv_.notify_all();
        }
        
        std::vector<pplx::task<void>> close_tasks;
        
        // Close all listeners
        for (auto& [path, listener] : listeners_) {
            if (listener != nullptr) {
                close_tasks.push_back(listener->close());
            }
        }
        
        // Wait for all listeners to close
        try {
            pplx::when_all(close_tasks.begin(), close_tasks.end()).wait();
            std::cout << "All listeners stopped successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error during stop: " << e.what() << std::endl;
            throw;
        }
    }

    // CORS helpers
    void Router::addCorsHeaders(web::http::http_response& response) {
        response.headers().add(U("Access-Control-Allow-Origin"), U("http://localhost:8000"));
        response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, PUT, DELETE, OPTIONS"));
        response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type, Authorization"));
        response.headers().add(U("Access-Control-Allow-Credentials"), U("true"));
        response.headers().add(U("Access-Control-Max-Age"), U("86400"));
    }

    void Router::replyWithCors(web::http::http_request request, web::http::status_code status, const web::json::value& body) {
        web::http::http_response response(status);
        response.set_body(body);
        addCorsHeaders(response);
        request.reply(response);
    }

    void Router::replyWithCors(web::http::http_request request, web::http::status_code status) {
        web::http::http_response response(status);
        addCorsHeaders(response);
        request.reply(response);
    }


} // namespace servuswelt
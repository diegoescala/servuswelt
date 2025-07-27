#include "router.h"

#include <unordered_map>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace web::http;
using namespace web::http::experimental::listener;

namespace servuswelt {
    // Static CORS origin storage
    static std::string g_cors_origin = "http://localhost:8000";
    
    Router::Router(const std::string& base_uri, const std::string& cors_origin):
        base_uri_(base_uri), cors_origin_(cors_origin), shutdown_requested_(false) {
        setCorsOrigin(cors_origin);
    }

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

            // Add route matching handler for all methods
            listener->support(route.getMethod(), [this, base_path](http_request request) {
                auto path = request.relative_uri().path();
                auto method = request.method();
                
                std::cout << "DEBUG: Request path=" << path << ", base_path=" << base_path << ", method=" << method << std::endl;
                
                // Find the best matching route
                auto match = findBestRoute(path, method, base_path);
                
                if (match.route != nullptr) {
                    std::cout << "DEBUG: Matched route: " << match.route->getPath() << std::endl;
                    // Store matched parameters in request for extraction by handlers
                    // Note: cpprest doesn't provide a clean way to pass custom data,
                    // so handlers need to do their own parameter extraction
                    match.route->getHandler()(request);
                } else {
                    std::cout << "DEBUG: No matching route found for path=" << path << ", base_path=" << base_path << std::endl;
                    // No matching route found
                    web::json::value error;
                    error[U("error")] = web::json::value::string("Not Found");
                    Router::replyWithCors(request, status_codes::NotFound, error);
                }
            });

            open_tasks_.push_back(listener->open());
            std::cout << "Listening for requests at " << listener->uri().to_string() << std::endl;
        } else {
            // Add CORS handling for OPTIONS requests if not already added
            it->second->support(methods::OPTIONS, [this](http_request request) {
                Router::replyWithCors(request, status_codes::OK);
            });

            // The route matching handler is already set up for this base path
            // All routes with the same base path will be handled by the same matcher
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
    void Router::setCorsOrigin(const std::string& cors_origin) {
        g_cors_origin = cors_origin;
    }
    
    void Router::addCorsHeaders(web::http::http_response& response) {
        std::cout << "Adding CORS headers with origin: " << g_cors_origin << std::endl;
        response.headers().add(U("Access-Control-Allow-Origin"), utility::conversions::to_string_t(g_cors_origin));
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

    // Route matching implementation
    Router::RouteMatch Router::findBestRoute(const std::string& path, web::http::method method, const std::string& base_path) const {
        RouteMatch best_match;
        
        for (const auto& route : routes_) {
            // Only consider routes with matching method
            if (route.getMethod() != method) {
                continue;
            }
            
            // Only consider routes that start with the same base path
            auto route_path = route.getPath();
            auto route_base = route_path.substr(0, route_path.find("/:"));
            if (route_base.empty()) {
                route_base = route_path;
            }
            
            if (route_base != base_path) {
                continue;
            }
            
            // Convert the full route path to a relative path for matching
            // e.g., route="/api/clients/list", base_path="/api/clients/list" -> relative_route="/"
            // e.g., route="/api/clients/:client_id", base_path="/api/clients" -> relative_route="/:client_id"
            std::string relative_route_path;
            if (route_path == base_path) {
                relative_route_path = "/";
            } else if (route_path.length() > base_path.length() && route_path.substr(0, base_path.length()) == base_path) {
                relative_route_path = route_path.substr(base_path.length());
                if (relative_route_path.empty() || relative_route_path[0] != '/') {
                    relative_route_path = "/" + relative_route_path;
                }
            } else {
                continue; // Route doesn't match base path
            }
            
            // Try to match this route using relative paths
            std::unordered_map<std::string, std::string> params;
            if (matchRoute(path, relative_route_path, params)) {
                int specificity = calculateSpecificity(relative_route_path);
                
                // Use this route if it's more specific than our current best
                if (specificity > best_match.specificity_score) {
                    best_match.route = &route;
                    best_match.params = params;
                    best_match.specificity_score = specificity;
                }
            }
        }
        
        return best_match;
    }

    bool Router::matchRoute(const std::string& path, const std::string& route_pattern, std::unordered_map<std::string, std::string>& params) const {
        // Split both path and pattern into segments
        auto split = [](const std::string& str) {
            std::vector<std::string> segments;
            std::stringstream ss(str);
            std::string segment;
            while (std::getline(ss, segment, '/')) {
                if (!segment.empty()) {
                    segments.push_back(segment);
                }
            }
            return segments;
        };
        
        auto path_segments = split(path);
        auto pattern_segments = split(route_pattern);
        
        // Must have same number of segments
        if (path_segments.size() != pattern_segments.size()) {
            return false;
        }
        
        params.clear();
        
        // Match each segment
        for (size_t i = 0; i < path_segments.size(); ++i) {
            const auto& path_seg = path_segments[i];
            const auto& pattern_seg = pattern_segments[i];
            
            if (pattern_seg.front() == ':') {
                // Parameter segment - extract parameter name and value
                std::string param_name = pattern_seg.substr(1); // Remove ':'
                params[param_name] = path_seg;
            } else {
                // Literal segment - must match exactly
                if (path_seg != pattern_seg) {
                    return false;
                }
            }
        }
        
        return true;
    }

    int Router::calculateSpecificity(const std::string& route_pattern) const {
        int specificity = 0;
        std::stringstream ss(route_pattern);
        std::string segment;
        
        while (std::getline(ss, segment, '/')) {
            if (!segment.empty()) {
                if (segment.front() == ':') {
                    // Parameter segment has lower specificity
                    specificity += 1;
                } else {
                    // Literal segment has higher specificity
                    specificity += 10;
                }
            }
        }
        
        return specificity;
    }


} // namespace servuswelt
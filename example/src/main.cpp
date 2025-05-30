#include "modules/transactions.h"
#include "modules/users.h"
#include <servuswelt/router.h>
#include <servuswelt/module.h>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/filestream.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/http_client.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <memory>
#include <csignal>

using namespace web::http;
using namespace web::http::experimental::listener;
using namespace web::http::client;

namespace {
    // Global pointer for signal handler, in an anonymous namespace
    Router* g_router = nullptr;
    volatile std::sig_atomic_t g_shutdown_requested = 0;
    
    void signal_handler(int signal) {
        std::cout << "\nReceived signal " << signal << ", initiating shutdown..." << std::endl;
        g_shutdown_requested = 1;
        if (g_router) {
            try {
                g_router->stop();
            } catch (const std::exception& e) {
                std::cerr << "Error during shutdown: " << e.what() << std::endl;
            }
        }
    }
}

void setupRoutes(Router& router, const std::vector<std::shared_ptr<Module>>& modules) {
    for (auto module : modules) {
        for (auto route : module->getRoutes()) {
            router.addRoute(route);
        }
    }
}

int main() {
    try {
        Router router("http://localhost:8080");
        g_router = &router;  // Set up global pointer

        // Set up signal handling
        std::signal(SIGINT, signal_handler);   // Handle Ctrl+C
        std::signal(SIGTERM, signal_handler);  // Handle termination request
        
        std::vector<std::shared_ptr<Module>> modules;
        modules.push_back(std::make_shared<Transactions>());
        modules.push_back(std::make_shared<Users>());
        
        setupRoutes(router, modules);
        
        std::cout << "Server starting. Press Ctrl+C to stop." << std::endl;
        
        try {
            router.startDispatch();
        } catch (const std::exception& e) {
            if (!g_shutdown_requested) {
                // Only log if it wasn't a requested shutdown
                std::cerr << "Error in dispatch: " << e.what() << std::endl;
                return 1;
            }
        }
        
        std::cout << "Server shutdown complete." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

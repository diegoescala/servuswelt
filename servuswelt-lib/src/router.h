#pragma once

#include "route.h"
#include "module.h"
#include <cpprest/http_listener.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <condition_variable>
#include <mutex>

namespace servuswelt {
    class Router {
        public:
            Router(const std::string& base_uri);
        virtual ~Router();

        void addModule(std::shared_ptr<Module> module);

        void addRoute(Route route);
        void handleRequest(web::http::http_request request);
        void startDispatch();
        void stop();

        inline bool isAlive() { return alive_; }

        // CORS helpers
        static void addCorsHeaders(web::http::http_response& response);
        static void replyWithCors(web::http::http_request request, web::http::status_code status, const web::json::value& body);
        static void replyWithCors(web::http::http_request request, web::http::status_code status);

    private:
        std::unordered_map<std::string, std::shared_ptr<web::http::experimental::listener::http_listener>> listeners_;
        std::vector<Route> routes_;
        std::vector<pplx::task<void>> open_tasks_;
        std::string base_uri_;

        bool alive_ = true;

        // For clean shutdown
        std::condition_variable shutdown_cv_;
        std::mutex shutdown_mutex_;
        bool shutdown_requested_ = false;

        // Helper function to add CORS headers
        void addCorsHeaders(web::http::http_request& request) {
            request.headers().add(U("Access-Control-Allow-Origin"), U("*"));
            request.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, PUT, DELETE, OPTIONS"));
            request.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type, Authorization"));
            request.headers().add(U("Access-Control-Max-Age"), U("86400")); // 24 hours
        }
    };
} // namespace servuswelt
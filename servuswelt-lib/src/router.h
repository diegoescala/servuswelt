#pragma once

#include "route.h"
#include "module.h"
#include <cpprest/http_listener.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <regex>

namespace servuswelt {
    class Router {
        public:
            Router(const std::string& base_uri, const std::string& cors_origin = "http://localhost:8000");
        virtual ~Router();

        void addModule(std::shared_ptr<Module> module);

        void addRoute(Route route);
        void handleRequest(web::http::http_request request);
        void startDispatch();
        void stop();

        inline bool isAlive() { return alive_; }

        // CORS helpers
        static void setCorsOrigin(const std::string& cors_origin);
        static void addCorsHeaders(web::http::http_response& response);
        static void replyWithCors(web::http::http_request request, web::http::status_code status, const web::json::value& body);
        static void replyWithCors(web::http::http_request request, web::http::status_code status);

    private:
        std::unordered_map<std::string, std::shared_ptr<web::http::experimental::listener::http_listener>> listeners_;
        std::vector<Route> routes_;
        std::vector<pplx::task<void>> open_tasks_;
        std::string base_uri_;
        std::string cors_origin_;

        bool alive_ = true;

        // For clean shutdown
        std::condition_variable shutdown_cv_;
        std::mutex shutdown_mutex_;
        bool shutdown_requested_ = false;

        // Route matching helpers
        struct RouteMatch {
            const Route* route;
            std::unordered_map<std::string, std::string> params;
            int specificity_score;
            
            RouteMatch() : route(nullptr), specificity_score(-1) {}
        };
        
        RouteMatch findBestRoute(const std::string& path, web::http::method method, const std::string& base_path) const;
        bool matchRoute(const std::string& path, const std::string& route_pattern, std::unordered_map<std::string, std::string>& params) const;
        int calculateSpecificity(const std::string& route_pattern) const;
    };
} // namespace servuswelt
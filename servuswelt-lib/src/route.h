#pragma once

#include <string>
#include <functional>
#include <cpprest/http_listener.h>

namespace servuswelt {
    class Route {
        public:
            Route(web::http::method method, std::string path, std::function<void(web::http::http_request)> handler);
        inline const std::string getPath() const { return path_; }
        inline const std::function<void(web::http::http_request)> getHandler() const { return handler_; } 
        inline const web::http::method getMethod() const { return method_; }

    private:
        std::string path_;
        web::http::method method_;
        std::function<void(web::http::http_request)> handler_;

        pplx::task<void> last_task_;
    };
}
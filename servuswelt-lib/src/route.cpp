#include "route.h"

namespace servuswelt {
    Route::Route(web::http::method method, std::string path, std::function<void(web::http::http_request)> handler):
    method_(method), path_(path), handler_(handler) {}
} // namespace servuswelt

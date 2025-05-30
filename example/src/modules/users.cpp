#include "users.h"

#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/filestream.h>
#include <cpprest/asyncrt_utils.h>

#include <servuswelt/router.h>

using namespace web;
using namespace web::http;
using namespace web::json;

using namespace servuswelt;

Users::Users()
{
}

Users::~Users()
{
}

const std::vector<Route> Users::getRoutes() const
{
    return {
        Route(methods::GET, "/api/users/:id", std::bind(&Users::getUser, this, std::placeholders::_1)),
        Route(methods::POST, "/api/users", std::bind(&Users::createUser, this, std::placeholders::_1)),
    };
}

void Users::getUser(web::http::http_request request) const
{
    auto path = request.relative_uri().path();
    auto id = path.substr(path.find_last_of('/') + 1);
    web::json::value response;
    web::json::value user_details;
    user_details["id"] = web::json::value::string(id);
    user_details["name"] = web::json::value::string("John Doe");
    user_details["email"] = web::json::value::string("john.doe@example.com");
    response["user_details"] = user_details;

    Router::replyWithCors(request, status_codes::OK, response);
}

void Users::createUser(web::http::http_request request) const
{
    auto body = request.extract_json().get();
    Router::replyWithCors(request, status_codes::OK, body);
}

#include "transactions.h"

#include <servuswelt/router.h>

#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/filestream.h>
#include <cpprest/asyncrt_utils.h>

#include <iostream>
#include <functional>

using namespace web;
using namespace web::http;
using namespace web::json;

Transactions::Transactions() 
{
}

Transactions::~Transactions()
{
}

const std::vector<Route> Transactions::getRoutes() const
{
    return {
        Route(methods::GET, "/api/transactions/:id", std::bind(&Transactions::getTransaction, this, std::placeholders::_1)),
        Route(methods::POST, "/api/transactions", std::bind(&Transactions::createTransaction, this, std::placeholders::_1)),
    };
}

void Transactions::getTransaction(web::http::http_request request) const
{
    auto path = request.relative_uri().path();
    auto id = path.substr(path.find_last_of('/') + 1);
    
    web::json::value response;
    web::json::value transaction_details;
    transaction_details["id"] = web::json::value::string(id);
    transaction_details["amount"] = web::json::value::number(4.25);
    transaction_details["date"] = web::json::value::string("2025-01-01");
    transaction_details["description"] = web::json::value::string("Espresso");
    response["transaction_details"] = transaction_details;
    Router::replyWithCors(request, status_codes::OK, response);
}

void Transactions::createTransaction(web::http::http_request request) const
{
    auto body = request.extract_json().get();
    Router::replyWithCors(request, status_codes::OK, body);
}

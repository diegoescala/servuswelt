#pragma once

#include <servuswelt/module.h>
#include <servuswelt/route.h>
#include <vector>
#include <memory>

class Transactions : public Module {
    public:
    Transactions();
    virtual ~Transactions();

    const std::vector<Route> getRoutes() const override;

    private:
    void getTransaction(web::http::http_request request) const;
    void createTransaction(web::http::http_request request) const;
};
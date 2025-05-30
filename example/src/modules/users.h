#pragma once

#include <servuswelt/module.h>
#include <servuswelt/route.h>

#include <cpprest/http_client.h>

class Users : public servuswelt::Module {
    public:
        Users();
        virtual ~Users();

        const std::vector<servuswelt::Route> getRoutes() const override;

    private:
        void getUser(web::http::http_request request) const;
        void createUser(web::http::http_request request) const;
};

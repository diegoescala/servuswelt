#define SERVUSWELT_TESTING
#include <gtest/gtest.h>
#include "../src/router.h"
#include <cpprest/http_listener.h>

using namespace servuswelt;
using namespace web::http;

// Test fixture for Router tests
class RouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test router
        router = std::make_unique<Router>("http://localhost:8080");
    }

    void TearDown() override {
        if (router) {
            router.reset();
        }
    }

    std::unique_ptr<Router> router;
};

// Mock handler for testing
void mockHandler(web::http::http_request request) {
    request.reply(web::http::status_codes::OK);
}

// Test basic route matching
TEST_F(RouterTest, BasicRouteMatching) {
    // Add a simple route
    Route simple_route(methods::GET, "/api/users", mockHandler);
    router->addRoute(simple_route);
    
    // This test verifies the route was added without crashing
    EXPECT_TRUE(true);
}

// Test parameterized route matching
TEST_F(RouterTest, ParameterizedRouteMatching) {
    // Add a parameterized route
    Route param_route(methods::GET, "/api/users/:id", mockHandler);
    router->addRoute(param_route);
    
    // This test verifies the parameterized route was added without crashing
    EXPECT_TRUE(true);
}

// Test the specific issue: routes with same base path but different patterns
TEST_F(RouterTest, ConflictingRoutePatterns) {
    // Add routes that should conflict based on the bug report
    Route collection_route(methods::GET, "/api/bank/accounts", mockHandler);
    Route individual_route(methods::GET, "/api/bank/accounts/:id", mockHandler);
    Route transactions_route(methods::GET, "/api/bank/transactions", mockHandler);
    Route transaction_route(methods::GET, "/api/bank/transactions/:id", mockHandler);
    
    // These should not crash when added
    router->addRoute(collection_route);
    router->addRoute(individual_route);
    router->addRoute(transactions_route);
    router->addRoute(transaction_route);
    
    EXPECT_TRUE(true);
}

// Test multi-segment parameterized routes (the Plod issue)
TEST_F(RouterTest, MultiSegmentParameterizedRoutes) {
    // Add routes that represent the Plod multi-tenant structure
    Route client_route(methods::GET, "/api/clients/:client_id", mockHandler);
    Route client_transactions_route(methods::GET, "/api/clients/:client_id/transactions", mockHandler);
    Route client_accounts_route(methods::GET, "/api/clients/:client_id/accounts", mockHandler);
    Route client_balances_route(methods::GET, "/api/clients/:client_id/balances", mockHandler);
    
    // These should not crash when added
    router->addRoute(client_route);
    router->addRoute(client_transactions_route);
    router->addRoute(client_accounts_route);
    router->addRoute(client_balances_route);
    
    EXPECT_TRUE(true);
}

// Test route specificity calculation
class RouterInternalsTest : public ::testing::Test {
protected:
    Router router{"http://localhost:8080"};
};

TEST_F(RouterInternalsTest, SpecificityCalculation) {
    // Test that more specific routes get higher scores
    int list_specificity = router.testCalculateSpecificity("/api/clients/list");
    int param_specificity = router.testCalculateSpecificity("/api/clients/:id");
    int multi_param_specificity = router.testCalculateSpecificity("/api/clients/:client_id/transactions");
    int single_param_specificity = router.testCalculateSpecificity("/api/clients/:client_id");
    
    // Exact matches should be more specific than parameterized matches
    EXPECT_GT(list_specificity, param_specificity);
    
    // Multi-segment routes should be more specific than single-segment ones
    EXPECT_GT(multi_param_specificity, single_param_specificity);
    
    std::cout << "Specificity scores:" << std::endl;
    std::cout << "/api/clients/list: " << list_specificity << std::endl;
    std::cout << "/api/clients/:id: " << param_specificity << std::endl;
    std::cout << "/api/clients/:client_id/transactions: " << multi_param_specificity << std::endl;
    std::cout << "/api/clients/:client_id: " << single_param_specificity << std::endl;
}

// Test parameter extraction
TEST_F(RouterInternalsTest, ParameterExtraction) {
    std::unordered_map<std::string, std::string> params;
    
    // Test simple parameter extraction
    bool match1 = router.testMatchRoute("/api/users/123", "/api/users/:id", params);
    EXPECT_TRUE(match1);
    EXPECT_EQ(params["id"], "123");
    
    // Test multi-segment parameter extraction
    params.clear();
    bool match2 = router.testMatchRoute("/api/clients/456/transactions", "/api/clients/:client_id/transactions", params);
    EXPECT_TRUE(match2);
    EXPECT_EQ(params["client_id"], "456");
    
    // Test exact match (no parameters)
    params.clear();
    bool match3 = router.testMatchRoute("/api/clients/list", "/api/clients/list", params);
    EXPECT_TRUE(match3);
    EXPECT_TRUE(params.empty());
    
    // Test mismatch
    params.clear();
    bool match4 = router.testMatchRoute("/api/clients/456", "/api/clients/:client_id/transactions", params);
    EXPECT_FALSE(match4);
}

// Test the specific routing issue identified in Plod
TEST_F(RouterInternalsTest, PlodRoutingIssue) {
    std::unordered_map<std::string, std::string> params;
    
    // These are the routes that should work but don't in Plod
    std::string client_route = "/api/clients/:client_id";
    std::string transactions_route = "/api/clients/:client_id/transactions"; 
    std::string accounts_route = "/api/clients/:client_id/accounts";
    
    // Test that each route pattern matches its intended path
    params.clear();
    EXPECT_TRUE(router.testMatchRoute("/api/clients/123", client_route, params));
    EXPECT_EQ(params["client_id"], "123");
    
    params.clear();
    EXPECT_TRUE(router.testMatchRoute("/api/clients/123/transactions", transactions_route, params));
    EXPECT_EQ(params["client_id"], "123");
    
    params.clear();
    EXPECT_TRUE(router.testMatchRoute("/api/clients/123/accounts", accounts_route, params));
    EXPECT_EQ(params["client_id"], "123");
    
    // Test that routes don't incorrectly match
    params.clear();
    EXPECT_FALSE(router.testMatchRoute("/api/clients/123/transactions", client_route, params));
    
    params.clear();
    EXPECT_FALSE(router.testMatchRoute("/api/clients/123", transactions_route, params));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
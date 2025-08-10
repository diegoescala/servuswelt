#define SERVUSWELT_TESTING
#include "../src/router.h"
#include <iostream>
#include <cassert>

using namespace servuswelt;
using namespace web::http;

void mockHandler(web::http::http_request request) {
    request.reply(web::http::status_codes::OK);
}

void testSpecificityCalculation() {
    std::cout << "Testing specificity calculation..." << std::endl;
    
    Router router("http://localhost:8080");
    
    int list_specificity = router.testCalculateSpecificity("/api/clients/list");
    int param_specificity = router.testCalculateSpecificity("/api/clients/:id");
    int multi_param_specificity = router.testCalculateSpecificity("/api/clients/:client_id/transactions");
    int single_param_specificity = router.testCalculateSpecificity("/api/clients/:client_id");
    
    std::cout << "Specificity scores:" << std::endl;
    std::cout << "/api/clients/list: " << list_specificity << std::endl;
    std::cout << "/api/clients/:id: " << param_specificity << std::endl;
    std::cout << "/api/clients/:client_id/transactions: " << multi_param_specificity << std::endl;
    std::cout << "/api/clients/:client_id: " << single_param_specificity << std::endl;
    
    // Exact matches should be more specific than parameterized matches
    assert(list_specificity > param_specificity);
    
    // Multi-segment routes should be more specific than single-segment ones
    assert(multi_param_specificity > single_param_specificity);
    
    std::cout << "✓ Specificity calculation tests passed!" << std::endl;
}

void testParameterExtraction() {
    std::cout << "\nTesting parameter extraction..." << std::endl;
    
    Router router("http://localhost:8080");
    std::unordered_map<std::string, std::string> params;
    
    // Test simple parameter extraction
    bool match1 = router.testMatchRoute("/api/users/123", "/api/users/:id", params);
    assert(match1);
    assert(params["id"] == "123");
    std::cout << "✓ Simple parameter extraction: /api/users/123 -> id=123" << std::endl;
    
    // Test multi-segment parameter extraction
    params.clear();
    bool match2 = router.testMatchRoute("/api/clients/456/transactions", "/api/clients/:client_id/transactions", params);
    assert(match2);
    assert(params["client_id"] == "456");
    std::cout << "✓ Multi-segment parameter extraction: /api/clients/456/transactions -> client_id=456" << std::endl;
    
    // Test exact match (no parameters)
    params.clear();
    bool match3 = router.testMatchRoute("/api/clients/list", "/api/clients/list", params);
    assert(match3);
    assert(params.empty());
    std::cout << "✓ Exact match with no parameters" << std::endl;
    
    // Test mismatch
    params.clear();
    bool match4 = router.testMatchRoute("/api/clients/456", "/api/clients/:client_id/transactions", params);
    assert(!match4);
    std::cout << "✓ Route mismatch correctly detected" << std::endl;
    
    std::cout << "✓ Parameter extraction tests passed!" << std::endl;
}

void testPlodRoutingIssue() {
    std::cout << "\nTesting Plod routing issue scenarios..." << std::endl;
    
    Router router("http://localhost:8080");
    std::unordered_map<std::string, std::string> params;
    
    // These are the routes that should work but don't in Plod
    std::string client_route = "/api/clients/:client_id";
    std::string transactions_route = "/api/clients/:client_id/transactions"; 
    std::string accounts_route = "/api/clients/:client_id/accounts";
    
    // Test that each route pattern matches its intended path
    params.clear();
    bool match1 = router.testMatchRoute("/api/clients/123", client_route, params);
    assert(match1);
    assert(params["client_id"] == "123");
    std::cout << "✓ Client route matches: /api/clients/123" << std::endl;
    
    params.clear();
    bool match2 = router.testMatchRoute("/api/clients/123/transactions", transactions_route, params);
    assert(match2);
    assert(params["client_id"] == "123");
    std::cout << "✓ Transactions route matches: /api/clients/123/transactions" << std::endl;
    
    params.clear();
    bool match3 = router.testMatchRoute("/api/clients/123/accounts", accounts_route, params);
    assert(match3);
    assert(params["client_id"] == "123");
    std::cout << "✓ Accounts route matches: /api/clients/123/accounts" << std::endl;
    
    // Test that routes don't incorrectly match
    params.clear();
    bool match4 = router.testMatchRoute("/api/clients/123/transactions", client_route, params);
    assert(!match4);
    std::cout << "✓ Transactions path does NOT match client route (correct)" << std::endl;
    
    params.clear();
    bool match5 = router.testMatchRoute("/api/clients/123", transactions_route, params);
    assert(!match5);
    std::cout << "✓ Client path does NOT match transactions route (correct)" << std::endl;
    
    std::cout << "✓ Plod routing issue tests passed!" << std::endl;
}

int main() {
    std::cout << "Running Servuswelt Router Tests\n" << std::endl;
    
    try {
        testSpecificityCalculation();
        testParameterExtraction();
        testPlodRoutingIssue();
        
        std::cout << "\n🎉 All tests passed! The router's internal matching logic works correctly." << std::endl;
        std::cout << "The issue must be in the HTTP listener setup or request handling." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
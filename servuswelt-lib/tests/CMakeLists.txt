# Test configuration for Servuswelt Router
cmake_minimum_required(VERSION 3.10)

# Find required packages
find_package(GTest REQUIRED)
find_package(cpprest REQUIRED)

# Create test executable
add_executable(router_test router_test.cpp)

# Link against the main servuswelt library and test dependencies
target_link_libraries(router_test 
    servuswelt-lib
    GTest::gtest 
    GTest::gtest_main
    cpprest
)

# Set C++ standard
set_property(TARGET router_test PROPERTY CXX_STANDARD 17)

# Add test to CTest
add_test(NAME RouterTest COMMAND router_test)

# Enable testing
enable_testing()
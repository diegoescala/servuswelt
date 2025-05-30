cmake_minimum_required(VERSION 3.10)

project(servuswelt-example)

set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(cpprestsdk REQUIRED)
find_package(servuswelt REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/modules/transactions.cpp src/modules/users.cpp)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${cpprestsdk_INCLUDE_DIRS}
    ${CPPREST_INCLUDE_DIR}  # Alternative variable name
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    servuswelt::servuswelt
    cpprestsdk::cpprest  # Add explicit link to cpprestsdk
    OpenSSL::SSL
    OpenSSL::Crypto
)
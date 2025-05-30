cmake_minimum_required(VERSION 3.10)
project(servuswelt)

set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME} STATIC src/router.cpp src/route.cpp )

# Make sure the library can find cpprestsdk headers
target_include_directories(${PROJECT_NAME} 
    PRIVATE ${cpprestsdk_INCLUDE_DIRS} ${CPPREST_INCLUDE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Link against cpprestsdk
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        ${Boost_LIBRARIES} 
        ${cpprestsdk_LIBRARIES}
        cpprestsdk::cpprest
        OpenSSL::SSL
        OpenSSL::Crypto
)

file(GLOB HEADER_FILES "src/*.h")
install(FILES ${HEADER_FILES} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} 
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Create and install CMake package configuration files
include(CMakePackageConfigHelpers)

# Ensure the cmake directory exists
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cmake")

# Create a version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

# Create a simple config file manually if template approach fails
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(cpprestsdk REQUIRED)
find_dependency(OpenSSL REQUIRED)
include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake\")
")

# Install the export set
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the config and version files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (PLATFORM_BINARY "platform_binary")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)


if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PLATFORM_BINARY} main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/service.cpp
                                 database/user.cpp
                                 database/order.cpp)


target_include_directories(${PLATFORM_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${PLATFORM_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${PLATFORM_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PLATFORM_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PLATFORM_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

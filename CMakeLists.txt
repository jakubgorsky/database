cmake_minimum_required(VERSION 3.23)
project(Database)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp)
set(HEADER_FILES
        headers/DBHandler.h)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/headers)
include_directories(${PROJECT_SOURCE_DIR}/includes)

add_executable(Database ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(Database "C:/libs/sqlite3/sqlite3.dll")
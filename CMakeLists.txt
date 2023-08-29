cmake_minimum_required(VERSION 3.26)
project(logparser)

set(CMAKE_CXX_STANDARD 23)

add_executable(logparser
        src/app/logparser.cpp
        src/app/macros.cpp

)

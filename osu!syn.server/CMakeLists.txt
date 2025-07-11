cmake_minimum_required(VERSION 3.31)
project(osu_syn_server)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(3rdparty)
include_directories(3rdparty/nlohmann)

add_executable(osu_syn_server
        3rdparty/nlohmann/json.hpp
        3rdparty/nlohmann/json_fwd.hpp
        3rdparty/httplib.h
        logger.cpp
        logger.hpp
        main.cpp
        server.cpp
        server.hpp)

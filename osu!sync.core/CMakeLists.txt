cmake_minimum_required(VERSION 3.20)
project(osu_sync_core)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(3rdpartyInclude)
include_directories(3rdpartyInclude/nlohmann)

add_executable(osu_sync_core
        3rdpartyInclude/nlohmann/json.hpp
        3rdpartyInclude/nlohmann/json_fwd.hpp
        3rdpartyInclude/httplib.h
        3rdpartyInclude/realm.h
        3rdpartyInclude/realm.hpp
        apiRequest.cpp
        apiRequest.hpp
        archiveManager.cpp
        archiveManager.hpp
        beatmapSetAttribute.hpp
        errcodes.hpp
        main.cpp
        osuInstance.cpp
        osuInstance.hpp
        systemUtils.hpp
        Logger.cpp
        Logger.h)
target_link_libraries(osu_sync_core PRIVATE
        ws2_32  # 添加这行以支持Windows平台上的socket通信
)

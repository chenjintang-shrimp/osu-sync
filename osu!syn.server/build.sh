#!/bin/bash

# 创建build目录
mkdir -p build

# 编译所有cpp文件
g++ -std=c++17 \
    -I. \
    -I3rdparty \
    main.cpp \
    logger.cpp \
    server.cpp \
    -o build/osu_sync_server \
    -pthread

# 如果编译成功，输出信息
if [ $? -eq 0 ]; then
    echo "编译成功！可执行文件位于 build/osu_sync_server"
else
    echo "编译失败！"
    exit 1
fi

# osu!sync

一个强大的 osu! 谱面同步和管理工具。

## 功能特点

- 📦 从已安装的 osu! 中导出谱面列表
- 🔄 支持多个镜像站的谱面下载
- 🚀 支持多线程并行下载（最高 50 线程）
- 💾 支持断点续传
- 🔧 灵活的命令行参数

如果你觉得这个工具对你有帮助：

- 欢迎提交 PR 来改进功能
- 有任何想法可以提 Issue
- 点个 Star 支持一下

## 编译要求

- Visual Studio 2022 或更高版本
- C++17 或更高版本
- CMake 3.15+
- vcpkg 包管理器

## 快速开始

### 导出谱面列表

从已安装的 osu! 中导出谱面列表到 JSON 文件：

```powershell
osu!sync.exe export "C:/Games/osu!" beatmaps.json
```

### 下载谱面列表

从服务器下载用户的谱面列表：

```powershell
osu!sync.exe download player123 http://sync-server.com
```

### 导入谱面

下载并导入谱面列表中的谱面：

```powershell
# 基本用法
osu!sync.exe import beatmaps.json ./downloads

# 指定 osu! 路径和并发数
osu!sync.exe import beatmaps.json ./downloads "C:/Games/osu!" 16

# 使用指定镜像站
osu!sync.exe import beatmaps.json ./downloads --mirror sayobot
```

## 镜像站支持

使用 `mirrors` 命令查看所有可用的镜像站：

```powershell
osu!sync.exe mirrors
```

目前支持的镜像站：

- Sayobot (sayobot)
- Catboy (catboy)
- Chimu (chimu)
- Nerinyan (nerinyan)
- Kitsu (kitsu)

可以通过 `--mirror` 参数指定使用的镜像站：

```powershell
osu!sync.exe import beatmaps.json ./downloads --mirror sayobot
```

## 命令行参数

### 全局命令

- `version`: 显示版本信息
- `mirrors`: 列出所有可用的镜像站

### export 命令

```powershell
osu!sync.exe export <osu路径> <输出文件>
```

- `<osu路径>`: osu! 游戏目录路径
- `<输出文件>`: 导出的 JSON 文件路径

### download 命令

```powershell
osu!sync.exe download <用户名> <服务器地址>
```

- `<用户名>`: 要下载谱面列表的用户名
- `<服务器地址>`: 同步服务器的地址

### import 命令

```powershell
osu!sync.exe import <谱面列表> <保存路径> [osu路径] [并发数] [--mirror <镜像站>]
```

- `<谱面列表>`: 要导入的谱面列表 JSON 文件
- `<保存路径>`: 谱面下载保存的目录
- `[osu路径]`: （可选）osu! 游戏目录路径
- `[并发数]`: （可选）同时下载的谱面数量，默认为 25
- `--mirror <镜像站>`: （可选）指定使用的镜像站

## 高级功能

### 并发下载

工具默认使用 25 个并发线程进行下载，可以通过参数调整：

```powershell
# 使用 16 个并发线程下载
osu!sync.exe import beatmaps.json ./downloads --mirror sayobot 16
```

### 断点续传

下载支持断点续传，意外中断后重新下载会从上次的位置继续。

### 进度显示

下载过程中会实时显示：

- 总体下载进度
- 当前活动下载数
- 每个文件的详细进度

## 环境变量

- `OSU_SYNC_MIRROR`: 设置自定义镜像站 URL

## 第三方依赖

- [cpp-realm](https://github.com/realm/realm-cpp) - Realm 的 C++ SDK
- [json](https://github.com/nlohmann/json) - 现代 C++ 的 JSON 处理库
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) - C++ HTTP/HTTPS 客户端/服务器库
- [aria2](https://github.com/aria2/aria2) - 高速下载工具

## 许可证

本项目基于 MIT 协议开源。

## 鸣谢

感谢以下镜像站提供的服务：

- [Sayobot](https://osu.sayobot.cn/)
- [Catboy](https://catboy.best/)
- [Chimu](https://chimu.moe/)
- [Nerinyan](https://nerinyan.moe/)
- [Kitsu](https://kitsu.moe/)

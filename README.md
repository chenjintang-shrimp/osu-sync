# osu!sync

一个可实现 osu! 数据同步的工具。

## 简介

这是一个全新的软件，除了可以同步你的几乎所有谱面以外，它还可以：

- 将你的谱面导出为一份清单，对同步进行方便的管理（WIP）
- 同步部分皮肤（WIP）

一切只需一个软件——osu!sync。

这个软件是我投入不少课余时间研发的，如果你们有任何想法：

- 如果能实现就提 PR
- 如果不能，就提 Issue

点个 Star，谢谢！

## 功能

详见[项目规划](https://github.com/users/jimmy-sketch/projects/3)

## 使用方法

### Core：命令行工具

程序提供三个主要功能：导出谱面列表、下载谱面列表和导入谱面。

基本用法：

```powershell
osu!sync <命令> [参数...]
```

#### 可用命令

1. **导出谱面列表** - 从本地 osu! 导出谱面列表到 JSON 文件

```powershell
osu!sync export <osu路径> <输出文件>
```

示例：

```powershell
osu!sync export "C:/Games/osu!" beatmaps.json
```

1. **下载谱面列表** - 从服务器下载用户的谱面列表

```powershell
osu!sync download <用户名> <服务器地址>
```

示例：

```powershell
osu!sync download player123 http://sync-server.com
```

1. **导入谱面** - 下载并导入谱面列表中的谱面

```powershell
osu!sync import <谱面列表> <保存路径> [osu路径]
```

示例：

```powershell
osu!sync import beatmaps.json ./downloads "C:/Games/osu!"
```

#### 参数说明

- `osu路径`：osu! 游戏的安装目录
- `保存路径`：下载谱面文件的临时存储目录
- 方括号 `[]` 中的参数为可选参数

## 第三方依赖

- [cpp-realm](https://github.com/realm/realm-cpp) - Realm 的 C++ SDK
- [json](https://github.com/nlohmann/json) - 现代 C++ 的 JSON 处理库
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) - C++ HTTP/HTTPS 客户端/服务器库
- [zlib-ng](https://github.com/zlib-ng/zlib-ng) - zlib 数据压缩库的下一代版本

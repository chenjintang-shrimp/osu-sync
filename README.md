# osu!sync，一个可实现 osu! 数据同步的工具

## 简介

这是一个全新的软件，除了可以同步你的几乎所有谱面以外，它还可以将你的谱面导出为一份清单，对要同步进行方便的管理（wip）,甚至可以同步部分皮肤（wip），一切只需一个软件----osu!sync。

这个软件是我投入不少课余时间研发的，所以求求你们用一用！同时，如果你们有任何想法，如果能实现就提pr，如果不能，就提issue。

点个star,谢谢！

## 功能
详见 https://github.com/users/jimmy-sketch/projects/3

## 使用方法
### core:命令行工具

程序提供三个主要功能：导出谱面列表、下载谱面列表和导入谱面。

基本用法：
```bash
osu!sync <命令> [参数...]
```

#### 可用命令：

1. **导出谱面列表** - 从本地 osu! 导出谱面列表到 JSON 文件：
```bash
osu!sync export <osu路径> <输出文件>
```
示例：`osu!sync export "C:/Games/osu!" beatmaps.json`

2. **下载谱面列表** - 从服务器下载用户的谱面列表：
```bash
osu!sync download <用户名> <服务器地址>
```
示例：`osu!sync download player123 http://sync-server.com`

3. **导入谱面** - 下载并导入谱面列表中的谱面：
```bash
osu!sync import <谱面列表> <保存路径> [osu路径]
```
示例：`osu!sync import beatmaps.json ./downloads "C:/Games/osu!"`

注意：
- `osu路径` 是 osu! 游戏的安装目录
- `保存路径` 是下载谱面文件的临时存储目录
- 方括号 `[]` 中的参数是可选的

## 引用项目：
- [cpp-realm by @realm](https://github.com/realm/realm-cpp)
- [json by @nlohmann](https://github.com/nlohmann/json)
- [cpp-httplib by @yhirose](https://github.com/yhirose/cpp-httplib)
- [zlib-ng by @zlib-ng](https://github.com/zlib-ng/zlib-ng)

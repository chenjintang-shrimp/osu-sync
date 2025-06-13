# 提交消息规范

为了生成更好的更新日志，我们使用[约定式提交](https://www.conventionalcommits.org/zh-hans/)规范。每个提交消息都应该遵循以下格式：

```
<类型>[可选的作用域]: <描述>

[可选的正文]

[可选的脚注]
```

## 类型

必须是以下类型之一：

- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档更新
- `style`: 代码格式（不影响代码运行的变动）
- `refactor`: 重构（既不是新增功能，也不是修复bug的代码变动）
- `perf`: 性能优化
- `test`: 增加测试
- `chore`: 构建过程或辅助工具的变动
- `revert`: 回滚到上一个版本

对于会导致破坏性变更的提交，可以在类型后添加 "!"，例如：

- `feat!`: 带有破坏性变更的新功能
- `fix!`: 带有破坏性变更的修复
这些提交会自动出现在更新日志的 BREAKING CHANGES 部分。

## 作用域

可选的作用域可以是：

- `downloader`: 下载器相关
- `core`: 核心功能
- `ui`: 用户界面
- `mirror`: 镜像站相关
- `deps`: 依赖更新

## 示例

```txt
feat(downloader): 添加断点续传功能

- 实现文件断点续传
- 添加下载进度显示
- 优化错误处理

Refs: #123
```

```txt
fix(mirror): 修复sayobot镜像站链接失效问题

将下载链接更新为最新的API地址

Closes #456
```

```txt
feat!(api): 重构镜像站API接口

- 完全重写了镜像站接口
- 移除了旧的URL生成方式
- 添加了新的错误处理机制

BREAKING CHANGE: 移除了旧版本的getMirrorUrl方法，请使用新的MirrorApi类代替
Closes #789
```

## 版本发布

当你要发布新版本时：

1. 确保所有代码已提交
2. 创建一个新的tag（例如 v1.0.1）：

   ```bash
   git tag -a v1.0.1 -m "release: v1.0.1"
   git push origin v1.0.1
   ```

GitHub Actions会自动：

- 构建项目
- 生成更新日志
- 创建发布
- 上传构建产物

## 更新日志生成规则

更新日志会自动包含以下内容：

- 所有新功能（feat）
- 所有修复（fix）
- 重大变更（BREAKING CHANGE）
- 代码重构（refactor）
- 构建和工具链变更（chore）

其他类型的提交（如docs, style, test等）默认不会出现在更新日志中。

各类型在更新日志中的分类：

- 🚀 Features: feat
- 🐛 Bug Fixes: fix
- ⚠️ BREAKING CHANGES
- 🔧 Internal Changes: refactor, chore

#include "ErrorCodes.h"

const char* GetErrorDescription(ModuleError err) {
    switch (err) {
        case ModuleError::Success: return "操作成功";
        case ModuleError::FileOpenFailed: return "无法打开插件描述文件";
        case ModuleError::JsonParseFailed: return "JSON解析错误";
        case ModuleError::CommandNotFound: return "命令未注册";
        case ModuleError::CommandHelpNotFound: return "命令在模块中未找到";
        case ModuleError::CommandExecutionFailed: return "命令执行失败";
        default: return "未知错误";
    }
}

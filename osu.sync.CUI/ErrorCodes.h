#pragma once
#include <string>

enum class ModuleError {
    Success = 0,
    FileOpenFailed,
    JsonParseFailed,
    CommandNotFound,
    CommandHelpNotFound,
    CommandExecutionFailed
};

const char* GetErrorDescription(ModuleError err);

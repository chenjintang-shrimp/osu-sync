# osu!sync Windows 编译脚本

param (
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",
    
    [ValidateSet("x64", "x86")]
    [string]$Platform = "x64",
    
    [switch]$Clean,
    [switch]$Rebuild,
    [switch]$VcpkgUpdate,
    [switch]$UseUTF8,
    [switch]$SkipServer
)

# 错误时停止执行
$ErrorActionPreference = "Stop"

# 定义颜色输出函数
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Write-Success($text) { Write-ColorOutput Green $text }
function Write-Info($text) { Write-ColorOutput Cyan $text }
function Write-Warning($text) { Write-ColorOutput Yellow $text }
function Write-Error($text) { Write-ColorOutput Red $text }

# 检查必要工具
Write-Info "正在检查构建环境..."

# 检查 VS 构建工具
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Error "错误: 找不到 Visual Studio。请安装 Visual Studio 2022 或更高版本。"
    exit 1
}

$vsPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -property installationPath
if (-not $vsPath) {
    Write-Error "错误: 找不到有效的 Visual Studio 安装。"
    exit 1
}

$msbuild = Join-Path $vsPath "MSBuild\Current\Bin\MSBuild.exe"
if (-not (Test-Path $msbuild)) {
    $msbuild = Join-Path $vsPath "MSBuild\15.0\Bin\MSBuild.exe"
}

# 检查第三方依赖
Write-Info "正在检查第三方依赖..."
$thirdPartyPath = "osu!sync.core\3rdpartyInclude"
if (-not (Test-Path $thirdPartyPath)) {
    Write-Error "错误: 找不到第三方库目录：$thirdPartyPath"
    exit 1
}

# 清理输出目录
if ($Clean -or $Rebuild) {
    Write-Info "正在清理输出目录..."
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue "x64"
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue "osu!sync.core\x64"
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue "beatmapDownloader\bin"
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue "beatmapDownloader\obj"
}

# 构建解决方案
Write-Info "正在构建 osu!sync (配置: $Configuration, 平台: $Platform)..."

# 还原 NuGet 包
Write-Info "正在还原 NuGet 包..."
& $msbuild "osu!sync.cpp.sln" /t:Restore /p:Configuration=$Configuration /p:Platform=$Platform

if ($LASTEXITCODE -ne 0) {
    Write-Error "NuGet 包还原失败"
    exit 1
}

$buildArgs = @(
    "osu!sync.cpp.sln",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    "/p:UseMultiToolTask=true",
    "/p:EnforceProcessCountAcrossBuilds=true"
)

# 添加 UTF-8 编译选项
if ($UseUTF8) {
    Write-Info "启用 UTF-8 编码编译..."
    $buildArgs += "/p:CharacterSet=Unicode"
    $buildArgs += "/p:UseUTF8=true"
}

# 跳过服务器项目构建
if ($SkipServer) {
    Write-Info "跳过服务器项目构建..."
    $buildArgs += "/p:BuildProjectReferences=false"
    # 使用项目GUID来指定构建目标
    $buildArgs += "/t:{6657d903-0642-42ac-b1bd-1e903944af44}"
}

$buildArgs += @(
    "/m",
    "/nologo",
    "/verbosity:minimal"
)

if ($Rebuild) {
    $buildArgs += "/t:Rebuild"
}

# 执行构建
& $msbuild $buildArgs

if ($LASTEXITCODE -ne 0) {
    Write-Error "构建失败！"
    exit $LASTEXITCODE
}

Write-Success "构建成功完成！"

# 输出文件位置
$outputPath = "$Platform\$Configuration"
Write-Info "输出文件位置: $outputPath"

# 复制依赖文件
Write-Info "正在复制运行时依赖..."
$targetDir = "x64\$Configuration"

# 创建目标目录
if (-not (Test-Path $targetDir)) {
    New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
}

# 复制 beatmapDownloader
Copy-Item -Force "beatmapDownloader\bin\$Configuration\net8.0\*" $targetDir

Write-Success "所有任务已完成！"

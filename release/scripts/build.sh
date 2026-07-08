#!/bin/bash
# 构建脚本 - MySSCOM
# 用法: ./build.sh [build_type]
# build_type: release | debug (默认: release)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
RELEASE_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_TYPE="${1:-release}"

# Qt 路径
QT_PATH="/Users/zuozl1992/Qt/6.8.3/macos"
CMAKE_PREFIX_PATH="$QT_PATH"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 构建应用
build_app() {
    local build_dir="$RELEASE_DIR/build/MySSCOM"
    
    log_info "开始构建 MySSCOM ($BUILD_TYPE)..."
    
    # 清理旧的构建目录
    rm -rf "$build_dir"
    mkdir -p "$build_dir"
    
    # 进入构建目录
    cd "$build_dir"
    
    # CMake 配置
    log_info "配置 CMake..."
    cmake "$PROJECT_ROOT" \
        -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0
    
    # 构建
    log_info "编译中..."
    cmake --build . --config "$BUILD_TYPE" -j$(sysctl -n hw.ncpu)
    
    log_info "MySSCOM 构建完成!"
    
    return 0
}

# 主函数
main() {
    log_info "=========================================="
    log_info "MySSCOM 构建脚本"
    log_info "构建类型: $BUILD_TYPE"
    log_info "=========================================="
    
    # 创建构建目录
    mkdir -p "$RELEASE_DIR/build"
    
    build_app
    
    log_info "=========================================="
    log_info "构建完成!"
    log_info "输出目录: $RELEASE_DIR/build/"
    log_info "=========================================="
}

main "$@"

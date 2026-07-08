#!/bin/bash
# 图标管理脚本 - MySSCOM
# 用法: ./manage_icons.sh [command]
# command: generate | verify (默认: verify)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

APP_NAME="MySSCOM"
ICON_SOURCE="$PROJECT_ROOT/resources/control.png"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 验证图标
verify_icons() {
    log_info "验证 $APP_NAME 图标..."
    
    if [ -f "$ICON_SOURCE" ]; then
        log_info "  ✓ control.png 存在"
        log_info "    路径: $ICON_SOURCE"
        
        # 检查图片尺寸
        local size=$(sips -g pixelWidth -g pixelHeight "$ICON_SOURCE" 2>/dev/null | grep -E "pixel" | awk '{print $2}')
        log_info "    尺寸: $size"
    else
        log_error "  ✗ control.png 不存在"
        log_error "    请将图标文件放到: $PROJECT_ROOT/resources/control.png"
        return 1
    fi
    
    local icns_file="$PROJECT_ROOT/resources/AppIcon.icns"
    if [ -f "$icns_file" ]; then
        log_info "  ✓ AppIcon.icns 存在"
        log_info "    路径: $icns_file"
    else
        log_warn "  ✗ AppIcon.icns 不存在，请运行: ./manage_icons.sh generate"
    fi
    
    log_info "图标验证通过!"
}

# 生成各尺寸图标
generate_icons() {
    log_info "生成 $APP_NAME 各尺寸图标..."
    
    if [ ! -f "$ICON_SOURCE" ]; then
        log_error "源图标不存在: $ICON_SOURCE"
        log_error "请将 control.png 放到 resources/ 目录"
        return 1
    fi
    
    local output_dir="$PROJECT_ROOT/resources/icons"
    mkdir -p "$output_dir"
    
    # 生成各尺寸图标
    log_info "生成各尺寸图标..."
    sips -z 16 16 "$ICON_SOURCE" --out "$output_dir/icon_16x16.png" 2>/dev/null
    sips -z 32 32 "$ICON_SOURCE" --out "$output_dir/icon_16x16@2x.png" 2>/dev/null
    sips -z 32 32 "$ICON_SOURCE" --out "$output_dir/icon_32x32.png" 2>/dev/null
    sips -z 64 64 "$ICON_SOURCE" --out "$output_dir/icon_32x32@2x.png" 2>/dev/null
    sips -z 128 128 "$ICON_SOURCE" --out "$output_dir/icon_128x128.png" 2>/dev/null
    sips -z 256 256 "$ICON_SOURCE" --out "$output_dir/icon_128x128@2x.png" 2>/dev/null
    sips -z 256 256 "$ICON_SOURCE" --out "$output_dir/icon_256x256.png" 2>/dev/null
    sips -z 512 512 "$ICON_SOURCE" --out "$output_dir/icon_256x256@2x.png" 2>/dev/null
    sips -z 512 512 "$ICON_SOURCE" --out "$output_dir/icon_512x512.png" 2>/dev/null
    cp "$ICON_SOURCE" "$output_dir/icon_512x512@2x.png"
    
    # 生成 icns 文件
    log_info "生成 AppIcon.icns..."
    local iconset_dir="$output_dir/$APP_NAME.iconset"
    rm -rf "$iconset_dir"
    mkdir -p "$iconset_dir"
    
    cp "$output_dir"/icon_*.png "$iconset_dir/"
    
    iconutil -c icns "$iconset_dir" -o "$output_dir/AppIcon.icns" 2>/dev/null || {
        log_warn "iconutil 命令失败，尝试使用 sips 生成 icns"
        sips -s format icns "$ICON_SOURCE" --out "$output_dir/AppIcon.icns" 2>/dev/null || true
    }
    
    # 清理 iconset 目录
    rm -rf "$iconset_dir"
    
    # 复制 icns 到 resources 根目录（供 CMake 使用）
    cp "$output_dir/AppIcon.icns" "$PROJECT_ROOT/resources/"
    
    log_info "图标生成完成!"
    log_info "输出目录: $output_dir"
    log_info "已复制 AppIcon.icns 到 resources/"
}

# 主函数
main() {
    local command="${1:-verify}"
    
    log_info "=========================================="
    log_info "MySSCOM 图标管理"
    log_info "=========================================="
    
    case "$command" in
        verify)
            verify_icons
            ;;
        generate)
            generate_icons
            ;;
        *)
            log_error "未知命令: $command"
            log_info "支持的命令: verify, generate"
            exit 1
            ;;
    esac
    
    log_info "=========================================="
}

main "$@"

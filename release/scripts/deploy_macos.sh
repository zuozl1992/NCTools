#!/bin/bash
# macOS 应用打包脚本 - MySSCOM
# 用法: ./deploy_macos.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RELEASE_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# 应用配置
APP_NAME="MySSCOM"
BUNDLE_ID="com.mysscom.app"
VERSION="2.0.0"

# 项目根目录
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 打包应用
deploy_app() {
    local build_dir="$RELEASE_DIR/build/MySSCOM"
    local app_bundle="$build_dir/src/app/MySSCOM.app"
    local output_dir="$RELEASE_DIR/output/MySSCOM"
    
    log_info "打包 $APP_NAME v$VERSION..."
    
    # 检查构建结果
    if [ ! -d "$app_bundle" ]; then
        log_error "应用 Bundle 不存在: $app_bundle"
        log_error "请先运行 build.sh 构建应用"
        return 1
    fi
    
    # 创建输出目录
    rm -rf "$output_dir"
    mkdir -p "$output_dir"
    
    # 复制应用到输出目录
    cp -R "$app_bundle" "$output_dir/"
    
    # 复制图标到输出目录
    cp "$PROJECT_ROOT/resources/control.png" "$output_dir/" 2>/dev/null || true
    cp "$PROJECT_ROOT/resources/AppIcon.icns" "$output_dir/" 2>/dev/null || true
    
    # 后续所有操作都在 output 目录进行
    local final_app="$output_dir/$APP_NAME.app"
    
    # 使用 macdeployqt 打包 Qt 依赖
    log_info "打包 Qt 依赖..."
    local qt_bin="/Users/zuozl1992/Qt/6.8.3/macos/bin"
    if [ -x "$qt_bin/macdeployqt" ]; then
        "$qt_bin/macdeployqt" "$final_app" -qmldir="$build_dir/src/app" 2>/dev/null || true
        log_info "Qt 依赖打包完成"
    else
        log_warn "macdeployqt 未找到，跳过 Qt 依赖打包"
    fi
    
    # 更新 Info.plist
    log_info "更新 Info.plist..."
    local plist="$final_app/Contents/Info.plist"
    if [ -f "$plist" ]; then
        /usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier $BUNDLE_ID" "$plist" 2>/dev/null || true
        /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString $VERSION" "$plist" 2>/dev/null || true
        /usr/libexec/PlistBuddy -c "Set :CFBundleVersion $VERSION" "$plist" 2>/dev/null || true
    fi
    
    # 重新签名
    log_info "重新签名..."
    # 先签名 Frameworks 下动态库
    if [ -d "$final_app/Contents/Frameworks" ]; then
        for dylib in "$final_app/Contents/Frameworks"/*.dylib; do
            if [ -f "$dylib" ] && [ ! -L "$dylib" ]; then
                codesign --force --sign - "$dylib" 2>/dev/null || true
            fi
        done
        # 签名 Qt framework
        for fw in "$final_app/Contents/Frameworks"/Qt*.framework; do
            if [ -d "$fw" ]; then
                codesign --force --sign - "$fw" 2>/dev/null || true
            fi
        done
    fi
    # 再签名整个 bundle
    codesign --force --sign - "$final_app"
    
    # 验证签名
    if codesign -vvv "$final_app" 2>&1 | grep -q "valid on disk"; then
        log_info "签名验证通过"
    else
        log_warn "签名可能有问题，但继续打包"
    fi
    
    # 移除隔离属性
    log_info "移除隔离属性..."
    xattr -cr "$final_app" 2>/dev/null || true
    
    # 创建 DMG
    log_info "创建 DMG 安装包..."
    local dmg_name="${APP_NAME}_v${VERSION}_$(date +%Y%m%d).dmg"
    local dmg_path="$output_dir/$dmg_name"
    
    # 使用 create-dmg 创建漂亮的 DMG
    if command -v create-dmg &> /dev/null; then
        create-dmg \
            --volname "$APP_NAME" \
            --volicon "$PROJECT_ROOT/resources/AppIcon.icns" \
            --window-pos 200 120 \
            --window-size 800 400 \
            --icon-size 100 \
            --icon "$APP_NAME.app" 200 190 \
            --hide-extension "$APP_NAME.app" \
            --app-drop-link 600 190 \
            "$dmg_path" \
            "$output_dir/$APP_NAME.app"
    else
        # 如果没有 create-dmg，使用原生方式
        hdiutil create -volname "$APP_NAME" \
            -srcfolder "$output_dir/$APP_NAME.app" \
            -ov -format UDZO \
            "$dmg_path"
    fi
    
    # 移除 DMG 隔离属性
    xattr -cr "$dmg_path" 2>/dev/null || true
    
    log_info "$APP_NAME 打包完成!"
    log_info "DMG 位置: $dmg_path"
    
    return 0
}

# 主函数
main() {
    log_info "=========================================="
    log_info "MySSCOM macOS 打包脚本"
    log_info "=========================================="
    
    deploy_app
    
    log_info "=========================================="
    log_info "打包完成! 输出目录: $RELEASE_DIR/output/"
    log_info "=========================================="
}

main "$@"

# MySSCOM 发布流程

## 概述

本文档定义了 MySSCOM 串口网络调试工具的发布流程、标准和规范。

## 目录结构

```
release/
├── README.md           # 本文档
├── QUICKSTART.md       # 快速参考
├── CHANGELOG.md        # 更新日志
├── .gitignore          # Git 忽略规则
├── scripts/            # 构建和部署脚本
│   ├── build.sh        # 构建脚本
│   ├── deploy_macos.sh # macOS 打包脚本
│   └── manage_icons.sh # 图标管理脚本
├── icons/              # 统一图标管理
│   └── MySSCOM/        # MySSCOM 图标资源
│       ├── AppIcon.icns
│       └── icon_*.png  # 各尺寸 PNG
├── build/              # 构建输出（临时）
└── output/             # 最终发布包
    └── MySSCOM/
```

## 发布流程

### 1. 准备阶段

#### 1.1 版本号更新

**MySSCOM**:
- 文件: `CMakeLists.txt` 第 2 行
- 格式: `project(MySSCOM VERSION X.Y.Z LANGUAGES CXX)`
- 文件: `src/app/main.cpp` 第 19 行
- 格式: `app.setApplicationVersion("X.Y.Z");`

#### 1.2 图标管理

**源图标文件**: `resources/control.png`

**生成 AppIcon.icns**:
```bash
cd release/scripts
./manage_icons.sh generate
```

此命令会：
1. 从 `control.png` 生成各尺寸 PNG 图标
2. 生成 `AppIcon.icns` 文件
3. 复制到 `resources/` 目录供 CMake 使用

**验证图标**:
```bash
./manage_icons.sh verify
```

### 2. 构建阶段

#### 2.1 使用构建脚本

```bash
cd release/scripts

# Release 构建
./build.sh release

# Debug 构建
./build.sh debug
```

#### 2.2 手动构建（如需调试）

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/Users/zuozl1992/Qt/6.8.3/macos -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 3. 打包阶段

#### 3.1 macOS 打包

```bash
cd release/scripts
./deploy_macos.sh
```

#### 3.2 打包内容

脚本自动完成:
1. 复制应用 Bundle 到输出目录
2. 使用 macdeployqt 打包 Qt 依赖
3. 更新 Info.plist 版本信息
4. 代码签名
5. 创建 DMG 安装包

### 4. 验证阶段

#### 4.1 验证应用

```bash
# 检查应用是否可启动
open release/output/MySSCOM/MySSCOM.app

# 检查图标是否正确
./manage_icons.sh verify
```

#### 4.2 验证清单

- [ ] 应用能正常启动
- [ ] 图标显示正确
- [ ] 版本号正确
- [ ] 串口通信正常
- [ ] TCP Server/Client 功能正常
- [ ] UDP 功能正常
- [ ] Hex 显示/发送正常
- [ ] 文件发送正常
- [ ] 快速发送面板正常
- [ ] 设置保存/加载正常
- [ ] 中英文切换正常

## 版本规范

### 版本号格式

采用语义化版本: `MAJOR.MINOR.PATCH`

- **MAJOR**: 不兼容的 API 修改
- **MINOR**: 向下兼容的功能性新增
- **PATCH**: 向下兼容的问题修正

### 版本号位置

| 文件 | 行号 | 格式 |
|------|------|------|
| CMakeLists.txt | 2 | `VERSION X.Y.Z` |
| src/app/main.cpp | 19 | `"X.Y.Z"` |

## 依赖要求

### 开发环境

- macOS 12.0+
- Xcode 14+ (Command Line Tools)
- CMake 3.19+
- Qt 6.8.3 (安装路径: `/Users/zuozl1992/Qt/6.8.3/macos`)

### 可选工具

- `create-dmg`: 创建美观的 DMG 安装包
  ```bash
  brew install create-dmg
  ```

## 常见问题

### Q: 构建失败

A: 检查 Qt 路径是否正确:
```bash
ls /Users/zuozl1992/Qt/6.8.3/macos/
```

### Q: 图标不显示

A: 检查图标文件是否正确同步:
```bash
./manage_icons.sh verify
./manage_icons.sh sync
```

### Q: 打包后应用无法启动

A: 检查 Qt 依赖是否正确打包:
```bash
otool -L release/output/MySSCOM/MySSCOM.app/Contents/MacOS/MySSCOM
```

## 发布检查清单

### 发布前

- [ ] 更新版本号
- [ ] 更新 CHANGELOG
- [ ] 运行测试
- [ ] 验证图标完整性

### 构建时

- [ ] 使用 Release 配置构建
- [ ] 检查编译警告
- [ ] 验证依赖库

### 打包后

- [ ] 验证应用可启动
- [ ] 验证图标显示
- [ ] 验证版本号
- [ ] 测试核心功能

### 发布时

- [ ] 命名规范: `MySSCOM_v{Version}_{YYYYMMDD}.dmg`
- [ ] 上传到发布平台
- [ ] 更新发布说明

## 更新日志

### 2026-07-07
- 创建统一发布流程
- 创建自动化构建脚本
- 创建图标管理脚本

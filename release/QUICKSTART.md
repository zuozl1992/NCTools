# 快速参考 - MySSCOM 发布流程

## 快速命令

### 构建
```bash
cd release/scripts

# Release 构建
./build.sh release

# Debug 构建
./build.sh debug
```

### 打包
```bash
cd release/scripts
./deploy_macos.sh
```

### 图标管理
```bash
cd release/scripts

# 验证图标
./manage_icons.sh verify

# 生成 AppIcon.icns（从 control.png）
./manage_icons.sh generate
```

## 版本号更新位置

1. `CMakeLists.txt` 第 2 行: `VERSION X.Y.Z`
2. `src/app/main.cpp` 第 19 行: `"X.Y.Z"`

## 输出目录

```
release/output/
└── MySSCOM/
    ├── MySSCOM.app
    └── MySSCOM_vX.Y.Z_YYYYMMDD.dmg
```

## 检查清单

### 发布前
- [ ] 更新版本号
- [ ] 验证图标完整性
- [ ] 运行测试

### 构建后
- [ ] 检查编译警告
- [ ] 验证依赖库

### 打包后
- [ ] 验证应用可启动
- [ ] 验证图标显示
- [ ] 验证版本号
- [ ] 测试核心功能

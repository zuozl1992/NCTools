# CHANGELOG

本文件记录 MySSCOM 的版本更新历史。

---

## [MySSCOM v2.0.0] - 2026-07-07

### 新增
- 全新 QML 界面，现代化 UI 设计
- 模块化架构重构（Core / Bridge / App 三层）
- 通信层抽象接口（AbstractTransport）
- 支持串口、TCP Server、TCP Client、UDP 四种通信方式
- 中英文国际化支持
- 快速发送面板（20 行独立条目）
- 文件发送功能（带进度条）
- TX/RX 字节统计
- Hex 显示/发送模式
- 定时自动发送
- 时间戳显示
- 设置持久化（QSettings）

### 改进
- 界面与逻辑完全分离
- 通信模块独立封装
- 统一的错误处理机制
- 异步文件发送（不阻塞 UI）

### 依赖
- Qt 6.8.3
- CMake 3.19+

---

## 发布说明

### 版本号规范
- MySSCOM: `MAJOR.MINOR.PATCH` (如 2.0.0)

### 文件命名规范
- DMG: `MySSCOM_v{Version}_{YYYYMMDD}.dmg`
- 示例: `MySSCOM_v2.0.0_20260707.dmg`

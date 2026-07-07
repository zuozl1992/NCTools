# MySSCOM Qt6.8.3 Modular Refactor

## 架构
- `src/backend/transportinterface.h`：统一传输接口
- `src/backend/serialtransport.*`：串口模块
- `src/backend/tcpclienttransport.*` / `tcpservertransport.*` / `udptransport.*`：网络模块
- `src/backend/transportmanager.*`：运行时连接管理
- `src/ui/qml/main.qml`：QML 界面

## 现有功能覆盖
- 串口、TCP Server/Client、UDP 全部保留
- HEX 发送、定时发送、发送后清空、扩展面板布局、文件与保存入口均保留
- QML 界面保留原工具体验，并拆分设置区、输出区、输入区、扩展抽屉

## 构建
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.8.3/gcc_64
cmake --build build
```

## 待续
1. 串口配置弹窗、保存对话框、文件传输对话框接入后端
2. 国际化 `MySSCOM_zh_CN.ts` 重新生成
3. 扩展面板记忆化持久配置

#pragma once

#include "transporttype.h"
#include "transportconfig.h"
#include "abstracttransport.h"
#include <memory>

/**
 * @brief 通信工厂类
 *        根据通信类型创建对应的通信对象实例
 */
class TransportFactory
{
public:
    /**
     * @brief 根据通信类型创建通信对象
     * @param type 通信类型枚举
     * @param parent 父对象
     * @return 通信对象智能指针
     */
    static std::unique_ptr<AbstractTransport> create(TransportType type, QObject *parent = nullptr);

    /**
     * @brief 根据通信配置创建通信对象
     * @param config 通信配置（variant类型）
     * @param parent 父对象
     * @return 通信对象智能指针
     */
    static std::unique_ptr<AbstractTransport> create(const TransportConfig &config, QObject *parent = nullptr);
};

#include "dataprocessor.h"
#include <QDateTime>
#include <QRegularExpression>

DataProcessor::DataProcessor(QObject *parent)
    : QObject(parent)
{
}

QString DataProcessor::toHexString(const QByteArray &data)
{
    // 转换为大写Hex字符串，用空格分隔
    return data.toHex(' ').toUpper();
}

QByteArray DataProcessor::fromHexString(const QString &hex)
{
    // 移除所有空白字符后转换
    QString cleaned = hex;
    cleaned.remove(QRegularExpression("\\s"));
    return QByteArray::fromHex(cleaned.toLocal8Bit());
}

QString DataProcessor::toLocalString(const QByteArray &data)
{
    return QString::fromLocal8Bit(data);
}

QString DataProcessor::appendTimestamp(const QString &text, bool isSend)
{
    // 格式：[时间戳]收/发:内容
    return QString("[%1]%2:%3")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
        .arg(isSend ? QStringLiteral("发") : QStringLiteral("收"))
        .arg(text);
}

bool DataProcessor::isValidHexInput(const QString &text, int cursorPos)
{
    if (cursorPos < 0 || cursorPos >= text.length())
        return true;

    // 检查字符是否为有效的Hex字符（0-9, A-F, a-f, 空格）
    QChar ch = text.at(cursorPos);
    return (ch >= '0' && ch <= '9')
        || (ch >= 'A' && ch <= 'F')
        || (ch >= 'a' && ch <= 'f')
        || (ch == ' ');
}

QString DataProcessor::filterHexString(const QString &text)
{
    // 过滤非Hex字符，只保留0-9, A-F, a-f
    QString result;
    for (const QChar &ch : text) {
        if ((ch >= '0' && ch <= '9')
            || (ch >= 'A' && ch <= 'F')
            || (ch >= 'a' && ch <= 'f')) {
            result.append(ch);
        }
    }
    return result;
}

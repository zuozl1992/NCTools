#include <QtTest>
#include <QCoreApplication>
#include "dataprocessor/dataprocessor.h"

class TestDataProcessor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testToHexString();
    void testFromHexString();
    void testToLocalString();
    void testAppendTimestamp();
    void testIsValidHexInput();
    void testFilterHexString();
};

void TestDataProcessor::initTestCase()
{
}

void TestDataProcessor::cleanupTestCase()
{
}

void TestDataProcessor::testToHexString()
{
    // Test basic ASCII
    QByteArray data1("Hello");
    QCOMPARE(DataProcessor::toHexString(data1), QString("48 65 6C 6C 6F"));

    // Test empty data
    QByteArray data2;
    QCOMPARE(DataProcessor::toHexString(data2), QString(""));

    // Test binary data
    QByteArray data3;
    data3.append('\x00');
    data3.append('\xFF');
    QCOMPARE(DataProcessor::toHexString(data3), QString("00 FF"));
}

void TestDataProcessor::testFromHexString()
{
    // Test basic hex string
    QCOMPARE(DataProcessor::fromHexString("48 65 6C 6C 6F"), QByteArray("Hello"));

    // Test without spaces
    QCOMPARE(DataProcessor::fromHexString("48656C6C6F"), QByteArray("Hello"));

    // Test with mixed case
    QCOMPARE(DataProcessor::fromHexString("48 65 6c 6c 6f"), QByteArray("Hello"));

    // Test empty string
    QCOMPARE(DataProcessor::fromHexString(""), QByteArray());
}

void TestDataProcessor::testToLocalString()
{
    // Test basic ASCII
    QByteArray data("Hello World");
    QCOMPARE(DataProcessor::toLocalString(data), QString("Hello World"));

    // Test empty data
    QCOMPARE(DataProcessor::toLocalString(QByteArray()), QString(""));
}

void TestDataProcessor::testAppendTimestamp()
{
    // Test send direction
    QString result1 = DataProcessor::appendTimestamp("test", true);
    QVERIFY(result1.contains("发"));
    QVERIFY(result1.contains("test"));
    QVERIFY(result1.startsWith("["));
    QVERIFY(result1.contains("]"));

    // Test receive direction
    QString result2 = DataProcessor::appendTimestamp("test", false);
    QVERIFY(result2.contains("收"));
    QVERIFY(result2.contains("test"));
}

void TestDataProcessor::testIsValidHexInput()
{
    // Valid hex characters
    QVERIFY(DataProcessor::isValidHexInput("0123456789ABCDEF", 0));
    QVERIFY(DataProcessor::isValidHexInput("0123456789ABCDEF", 15));
    QVERIFY(DataProcessor::isValidHexInput("AB CD", 2)); // space

    // Invalid characters
    QVERIFY(!DataProcessor::isValidHexInput("GH", 0));
    QVERIFY(!DataProcessor::isValidHexInput("xyz", 0));

    // Out of bounds
    QVERIFY(DataProcessor::isValidHexInput("", 0));
    QVERIFY(DataProcessor::isValidHexInput("AB", -1));
    QVERIFY(DataProcessor::isValidHexInput("AB", 5));
}

void TestDataProcessor::testFilterHexString()
{
    // Test filtering valid hex
    QCOMPARE(DataProcessor::filterHexString("AB CD EF"), QString("ABCDEF"));

    // Test filtering mixed content
    QCOMPARE(DataProcessor::filterHexString("A1B2 C3D4"), QString("A1B2C3D4"));

    // Test filtering with invalid characters
    QCOMPARE(DataProcessor::filterHexString("A1B2!@#C3D4"), QString("A1B2C3D4"));

    // Test empty string
    QCOMPARE(DataProcessor::filterHexString(""), QString(""));
}

QTEST_MAIN(TestDataProcessor)
#include "test_dataprocessor.moc"

#include <QApplication>
#include <QMainWindow>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QTranslator>
#include <QLocale>
#include <QIcon>

#include "src/backend/transportinterface.h"
#include "src/backend/serialtransport.h"
#include "src/backend/tcpclienttransport.h"
#include "src/backend/tcpservertransport.h"
#include "src/backend/udptransport.h"
#include "src/backend/transportmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/control.png"));
    app.setApplicationName("MySSCOM");
    app.setApplicationVersion("2.0");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MySSCOM_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    qmlRegisterUncreatableType<TransportInterface>("MySSCOM", 1, 0, "TransportInterface", "Abstract transport interface");
    qmlRegisterType<SerialTransport>("MySSCOM", 1, 0, "SerialTransport");
    qmlRegisterType<TcpClientTransport>("MySSCOM", 1, 0, "TcpClientTransport");
    qmlRegisterType<TcpServerTransport>("MySSCOM", 1, 0, "TcpServerTransport");
    qmlRegisterType<UdpTransport>("MySSCOM", 1, 0, "UdpTransport");
    qmlRegisterType<TransportManager>("MySSCOM", 1, 0, "TransportManager");

    QMainWindow window;
    window.setWindowTitle(QObject::tr("MySSCOM - 串口网络调试工具"));

    auto *centralWidget = new QWidget(&window);
    auto *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *quickWidget = new QQuickWidget(centralWidget);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    TransportManager transportManager;
    auto *serialTransport = new SerialTransport(&transportManager);
    auto *tcpClientTransport = new TcpClientTransport(&transportManager);
    auto *tcpServerTransport = new TcpServerTransport(&transportManager);
    auto *udpTransport = new UdpTransport(&transportManager);

    transportManager.registerTransport(serialTransport);
    transportManager.registerTransport(tcpClientTransport);
    transportManager.registerTransport(tcpServerTransport);
    transportManager.registerTransport(udpTransport);

    quickWidget->rootContext()->setContextProperty("transportManager", &transportManager);
    quickWidget->rootContext()->setContextProperty("serialTransport", serialTransport);
    quickWidget->rootContext()->setContextProperty("tcpClientTransport", tcpClientTransport);
    quickWidget->rootContext()->setContextProperty("tcpServerTransport", tcpServerTransport);
    quickWidget->rootContext()->setContextProperty("udpTransport", udpTransport);

    quickWidget->setSource(QUrl("qrc:/qt/qml/MySSCOM/src/ui/qml/main.qml"));
    layout->addWidget(quickWidget);

    window.setCentralWidget(centralWidget);
    window.resize(1080, 720);
    window.show();

    return QApplication::exec();
}

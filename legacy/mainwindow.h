#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class QUdpSocket;
class QTcpSocket;
class QTcpServer;
class QSerialPort;
class QLineEdit;
class QCheckBox;
class QWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ConnectType{
        NullDev,
        Serial,
        TcpServer,
        TcpClient,
        Udp
    };
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void udpReadyReadSlot();
    void tcpClientReadyReadSlot();
    void tcpClientDisconnectedSlot();
    void tcpServerNewConnectionSlot();
    void tcpServerSocketReadyReadSlot();
    void tcpServerSocketDisconnectedSlot();
    void serialReadyReadSlot();
    void on_cbName_currentIndexChanged(int index);

    void on_btnSend_clicked();

    void on_btnUdpBind_clicked();

    void on_cbHexSend_clicked(bool checked);

    void on_teSendData_textChanged();

    void on_btnTcpClientConnect_clicked();

    void on_swMain_currentChanged(int arg1);

    void on_btnTcpServerStart_clicked();

    void on_btnSerialOpen_clicked();

    void on_btnSerialMore_clicked();

    void on_btnAddManualName_clicked();

    void on_btnClear_clicked();

    void on_btnFlushNameList_clicked();

    void on_cbAutoClearSend_checkStateChanged(const Qt::CheckState &arg1);

    void on_cbSendAuto_checkStateChanged(const Qt::CheckState &arg1);

    void on_btnOpenFile_clicked();

    void on_btnSendFile_clicked();

    void on_btnSave_clicked();

    void on_btnExp_clicked();

    void on_actionAbout_triggered();

private:
    void initPort();
    void initExpPanel();
    void saveSettings();
    void loadSettings();
    ConnectType getNowType();
    bool sendUseUdp(const QByteArray &data);
    bool sendUseTcpClient(const QByteArray &data);
    bool sendUseTcpServer(const QByteArray &data);
    bool sendUseSerial(const QByteArray &data);
    void execWarnMsg(QString msg);
    void showDataToUI(const QByteArray &data, int dir=0);
    void closeUdp();
    void closeTcpClient();
    void closeTcpServer();
    void closeSerialport();
    void timerEvent(QTimerEvent *event) override;
    bool autoSendData(const QByteArray &sendData);
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpClientSocket;
    QTcpServer *tcpServer;
    QSerialPort *serial;
    QList<QTcpSocket *> tcpServerSocketList;
    QString normalStr, hexStr;
    int autoSendTimer = -1;
    QWidget *expPanel;
    QVector<QLineEdit *> expLineEdits;
    QVector<QCheckBox *> expHexChecks;
};
#endif // MAINWINDOW_H

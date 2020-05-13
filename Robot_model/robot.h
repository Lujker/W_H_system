#ifndef ROBOT_H
#define ROBOT_H

#include <QMainWindow>
#include <QObject>
#include <QTcpSocket>
#include <QTime>
#include <QBitArray>
#include <QIODevice>
#include <QDataStream>
#include <QDebug>
#include <vector>
#include <chrono>
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui { class Robot; }
QT_END_NAMESPACE

enum charging_qr {CHARG1=10001,CHARG2=20013,CHARG3=30025}; //Qr-коды станций зарядки
enum client_qr {CLIENT1=40037,CLIENT2=50049,CLIENT3=60061}; //Qr-коды для доставки клиентам


class Robot : public QMainWindow
{
    Q_OBJECT

public:
    Robot(QWidget *parent = nullptr);
    ~Robot();
public slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer(int qr_code,bool without_flag);
    void slotConnected();

    void come_to_path(bool without_flag);

private:
    QTcpSocket* m_pTcpSocket;
    quint16 m_nNextBlockSize;
    int           current_qr; //последняя метка считанная роботом
    bool             in_path; //если робот в пути = true
    std::vector<int>    path;
    Ui::Robot *ui;
};



#endif // ROBOT_H

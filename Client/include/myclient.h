#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QTime>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QLabel>
#include <QDebug>
#include <vector>
#include "packet.h"
#include "openserv.h"
#include <QStatusBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MyClient; }
QT_END_NAMESPACE

enum mas_status {DELIVER,ACCEPT,CONNECT}; //статусы сообщений клиента

class MyClient : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    quint16 m_nNextBlockSize;
    QVector<Packet> pack;
    QStandardItemModel* model, *model2;
    QString Name; //имя клиента
    int pass; //пароль клиента
    QString last_serv_ip;
    bool is_connected;    

public:
    MyClient(QWidget *parent = nullptr);
    ~MyClient();
    void addpacket(Packet& n_p);
public slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer(int flag);
    void slotConnected();
    void disconnect();

private slots:
    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void open_serv();
    void makerequest (const QString &ip, const QString &name, const QString &pass);


    void on_Connect_Serv_clicked();

private:
    Ui::MyClient *ui;
};
#endif // MYCLIENT_H

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkInterface>
#include "QStandardItemModel"
#include "QStandardItem"

namespace Ui {
class Connection;
}

class Connection : public QMainWindow
{
    Q_OBJECT

public:
    explicit Connection(QTcpServer* serv, std::vector<QTcpSocket*>*    connection, QWidget *parent = nullptr);
    ~Connection();
    void set_con_model(QTcpSocket* soc);
    void del_soc_from_model(int row);
    void update_table();
    bool is_ready;

private slots:
    void on_disconnect_clicked();

signals:
    void dis_soc(int row);

private:
    Ui::Connection *ui;
    std::vector<QTcpSocket*>*    connection;
    QTcpServer* serv;
    QStandardItemModel*    model;

};

#endif // CONNECTION_H

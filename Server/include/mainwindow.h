#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QTime>
#include <QBitArray>
#include <QIODevice>
#include <QDataStream>
#include "QStandardItemModel"
#include "QStandardItem"
#include <algorithm>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "packet.h"
#include "database.h"
#include "connection.h"
#include "map.h"
#include "qr.h"

#define QR_NUMB 72

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum mas_status {DELIVER,ACCEPT,CONNECT}; //статусы сообщений клиента
enum pass {FIRST=1111, SECOND=2222, THIRD =3333}; //пароли

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
   MainWindow(int nPort, QWidget *parent = nullptr);
   ~MainWindow();
   void addpacket(Packet& n_p);
   void delpacket(Packet& n_p);
   int get_qr_post(int numb_post);
   void set_map();

public slots:
   virtual void slotNewConnection();
   void slotReadClient   ();
//   void deleter();

private slots:
   void on_DataBase_clicked(); //открывает окно базы данных
   void on_Connection_clicked(); //открывает окно соединений
   void on_Map_clicked();

   void on_GetPacket_clicked(); //формирует пакет из БД (эмоляция приема пакетов)
   void dell_soc (int row); //отключает выбранного в окне connection клиента   

private:
   void sendToClient(QTcpSocket* pSocket, Packet& n_p);
   size_t con_size();
   QTcpSocket *get_random_con();
   int chek_pass_on_map(int pass);

    std::vector<Packet>               pack;
    std::vector<QTcpSocket*>    connection;
    std::vector<QR>                 qr_map;   
    quint16               m_nNextBlockSize;
    QSqlDatabase                  database;
    Map*                                mp;
    QTcpServer*               m_ptcpServer;
    QStandardItemModel*              model;
    Ui::MainWindow*                     ui;
    DataBase*                           db;
    Connection*                        con;



};
#endif // MAINWINDOW_H

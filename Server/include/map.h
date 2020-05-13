#ifndef MAP_H
#define MAP_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QTableView>
#include <QNetworkInterface>
#include <QTime>
#include "QStandardItemModel"
#include "QStandardItem"
#include <chrono>
#include <thread>
#include "qr.h"
#include <QPair>

enum charging_qr {CHARG1=10001,CHARG2=20013,CHARG3=30025}; //Qr-коды станций зарядки
enum client_qr {CLIENT1=40037,CLIENT2=50049,CLIENT3=60061}; //Qr-коды для доставки клиентам

namespace Ui {
class Map;
}

class Map : public QMainWindow
{
    Q_OBJECT

public:
    explicit Map(QSqlDatabase *db, std::vector<QR>* qr_map, QWidget *parent = nullptr);
    ~Map();
    void setup_map();//заполняет таблицу номерами стелажей и qr-метками
    void build_way(int qr_code, int sel_robot);//строит путь до выбранной метки
    int selected_robot(int qr_code); // выбор робота для доставки покета (вызывается в build_way)
    void send_to_pack(int numb_table, int qr_deliv); //принимает номер стелажа который нужно доставить и куда нужно доставить
    void send_to_client(int numb_table, int qr_deliv);
    void get_out_table(int numb_table, int qr_deliv);//отвозит стелаж на его место от клиента
    QList<int>          client_on_map;

private slots:
   void on_Send_clicked();
   virtual void slotNewConnection();
   void         slotReadClient   ();
   void on_SendToCharging_clicked();

private:
   void sendToClient(QTcpSocket* pSocket, int qr_code, int sel_robot,bool without_flag); //отправляет сокет путь до нужной qr-метки
    Ui::Map*                              ui;
    QSqlQuery*                         query;
    QSqlDatabase*                         db;
    quint16                 m_nNextBlockSize;
    QSqlTableModel*                    model;
    QStandardItemModel*         model_robots;
    QTcpServer*                Robots_server;
    std::vector<QTcpSocket*>      connection; //массив сокетов (соединений с роботами
    std::vector<std::vector<int>*> robo_path; //массив из массивов qr-кодов (массив путей)
    std::vector<int>             current_pos; //текущее положение каждого робота
    std::vector<QR>*                  qr_map; //указатель на основной масстив карты qr-кодов
    std::vector<QPair<int,int>>              rob_cl;


};

#endif // MAP_H

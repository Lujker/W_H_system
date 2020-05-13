#ifndef DATABASE_H
#define DATABASE_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QSqlError>
#include <QFileDialog>
#include <QDebug>
#include <QTableView>
#include "sql_code.h"
#include "packet.h"
#include "qr.h"

namespace Ui {
class DataBase;
}

class DataBase : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataBase(QSqlDatabase *db, std::vector<QR>* qr_map, QWidget *parent = nullptr);
    ~DataBase();
    bool del_packet_from_db(Packet& pc);
    Packet get_packet();
    unsigned   count_pack_of_client;
private slots:
    void on_add_row_box_clicked();
    void on_reselect_box_clicked();
    void on_revert_box_clicked();
    void on_delete_row_box_clicked();
    void on_submit_box_clicked();
    void on_dialog_search_clicked();
    void on_request_clicked();
    void makerequest (const QString &filepath);
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_Boutton_clear_clicked();



private:
    QString               path;
    QSqlDatabase*           db;
    QSqlTableModel*      model;
    QSqlTableModel* sort_model;
    QSqlQuery*           query;
    Ui::DataBase*           ui;
    std::vector<QR>*    qr_map;

};

#endif // DATABASE_H

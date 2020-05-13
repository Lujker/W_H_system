#include "connection.h"
#include "ui_connection.h"

Connection::Connection(QTcpServer* serv , std::vector<QTcpSocket*>*    connection, QWidget *parent) :
    QMainWindow(parent),serv(serv), connection(connection), is_ready(false),
    ui(new Ui::Connection)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    QStringList horizontalHeader1;
        horizontalHeader1.append("          Name          ");
        horizontalHeader1.append("                    IP                    ");
        horizontalHeader1.append("   Port   ");
    QStringList verticalHeader1;

    model->setHorizontalHeaderLabels(horizontalHeader1);
    model->setVerticalHeaderLabels(verticalHeader1);

    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ///////////////////////////////////////////////////////////////
   is_ready=true; //разрешаем добавлять новых пользователей в таблицу из главного окна при новых подключениях

}

Connection::~Connection()
{
    delete ui;
}

void Connection::set_con_model(QTcpSocket *soc)
{
    QStandardItem *item;
    int index = model->rowCount();
    item = new QStandardItem(soc->objectName());
    model->setItem(index,0,item);


    QString s = soc->peerAddress().toString();
    auto parts = s.split("::ffff:");
    item = new QStandardItem(parts[parts.size()-1]);
    model->setItem(index,1,item);
    item = new QStandardItem( QString::number(soc->peerPort()));
    model->setItem(index,2,item);
}

void Connection::del_soc_from_model(int row)
{
    qDebug()<<"Del soc form model row: "<< row;
    model->removeRow(row);
}

void Connection::update_table()
{
    QStandardItem *item;

    for(int i=0;i<static_cast<int>(connection->size());i++){
    for(int x=0;x<=model->columnCount();x++){
        if(connection->at(i)!=nullptr){
        if(x==0){
        item = new QStandardItem(connection->at(i)->objectName());
        model->setItem(i,0,item);}
        if(x==1){
            QString s = connection->at(i)->peerAddress().toString();
            auto parts = s.split("::ffff:");
            item = new QStandardItem(parts[parts.size()-1]);
//        item = new QStandardItem(connection->at(i)->peerAddress().toString());
        model->setItem(i,1,item);}
        if(x==2){
        item = new QStandardItem(QString::number(connection->at(i)->peerPort()));
        model->setItem(i,2,item);}
        }
    }
    }

}

void Connection::on_disconnect_clicked()
{
    qDebug()<<ui->tableView->currentIndex().row();
    if(connection->at(ui->tableView->currentIndex().row())->isValid())
    emit dis_soc(ui->tableView->currentIndex().row()); //генерируем сигнал номером сокета для отключения
    else model->removeRow(ui->tableView->currentIndex().row());
    qDebug()<<"Exit from disc_soc";
}


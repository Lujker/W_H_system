#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int nPort, QWidget *parent)
    : QMainWindow(parent), m_nNextBlockSize(0),
      connection(std::vector<QTcpSocket*>(3,nullptr)),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_ptcpServer = new QTcpServer(this); // создаем сервер при открытии программы
    if(!m_ptcpServer->listen(QHostAddress::Any, nPort)){
        QMessageBox::critical(nullptr,
                              "Server Error",
                              "Unable to start the server: " + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }

    ui->textEdit->appendPlainText(QTime::currentTime().toString()+": Welcom to server!");
   QList<QHostAddress> addressList = QNetworkInterface::allAddresses();

        auto adress = addressList.begin()+1;
//        ui->textEdit->appendPlainText(QTime::currentTime().toString()+": Server address is: " +  adress->toString());
        QString text = QTime::currentTime().toString()+": Server address is: " +  adress->toString();
           QTextCharFormat tf = ui->textEdit->currentCharFormat();
           tf.setForeground(QBrush(Qt::green)); ui->textEdit->setCurrentCharFormat(tf);
              ui->textEdit->appendPlainText(text);
              tf.setForeground(QBrush(Qt::black)); ui->textEdit->setCurrentCharFormat(tf);


    connect(m_ptcpServer, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    con = new Connection(m_ptcpServer,&connection, this); //создание класса connection для управления соединениями

    model = new QStandardItemModel(this);
    QStringList horizontalHeader1;
        horizontalHeader1.append("Время получения пакета");
        horizontalHeader1.append("              Номер пакета              ");
    QStringList verticalHeader1;

    model->setHorizontalHeaderLabels(horizontalHeader1);
    model->setVerticalHeaderLabels(verticalHeader1);

    ui->tableView->setModel(model);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ///////////////////////////////////////запуск бд/////////////////////
    database = QSqlDatabase::addDatabase("QSQLITE"); //подключаем драйвер для sqlite
    database.setDatabaseName("warehouse.db"); //открываем базу по умолчанию
    if(!database.open()){ //если не открылось выводим причину ошибки
        qDebug()<< database.lastError().text();
        QMessageBox::critical(nullptr,
                              "Database Error",
                              "Unable to start the database: " + database.lastError().text());
        return;
    }
    else {
        qDebug()<<"Complete";
    }
    set_map();

    ui->textEdit->appendPlainText(QTime::currentTime().toString()+": QR-map is setup!");
    db = new DataBase(&database, &qr_map, this);
    mp = new Map(&database, &qr_map, this);
    db->count_pack_of_client=0;

    ui->Map->setStyleSheet ("text-align: left;");
    ui->Connection->setStyleSheet ("text-align: left;");
    ui->DataBase->setStyleSheet ("text-align: left;");
    connect(con, &Connection::dis_soc, this, &MainWindow::dell_soc);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//добавление новых пакетов в таблицу
void MainWindow::addpacket(Packet &n_p)
{
    QStandardItem *item;
    int index = model->rowCount();
    item = new QStandardItem(n_p.time_of_accept.toString());
    model->setItem(index,0,item);
    item = new QStandardItem(QString::fromStdString(std::to_string(n_p.number_of_packet)));
    model->setItem(index,1,item);
}

void MainWindow::delpacket(Packet &n_p)
{
    for(int i=0;i<=model->columnCount();i++){
    for(int x=0;x<=model->rowCount();x++){
    QModelIndex ind = model->index(x,i);
    if(ind.data().toInt()==n_p.number_of_packet){
    model->removeRow(ind.row());
    break;
    }
    }}
}
//одна из главных функций заполнения карты qr-кодов, пользуемся этой картой дальше и в DB ,и в Map
void MainWindow::set_map()
{
    int n_p=1;
    for(int i=0;i<6;++i){
        for(int j=0;j<12;++j){
            if((i==0)||(i==2)||(i==3)||(i==5)){
                if(j>=3&&j<=12){
                    QR new_qr;
                    new_qr.qr_state=10000*(i+1)+(12*i)+(j+1);
                    new_qr.numb_post=n_p;
                    new_qr.qr_post=10000*(i+1)+n_p;
                    qr_map.push_back(new_qr);
                    ++n_p;
                }
                else{
                    QR new_qr;
                    new_qr.qr_state=10000*(i+1)+(12*i)+(j+1);
                    new_qr.numb_post=0;
                    new_qr.qr_post=0;
                    qr_map.push_back(new_qr);
                }
              }
            else{
                QR new_qr;
                new_qr.qr_state=10000*(i+1)+(12*i)+(j+1);
                new_qr.numb_post=0;
                new_qr.qr_post=0;
                qr_map.push_back(new_qr);
            }
        }
}
}


void MainWindow::slotNewConnection()
{
    size_t i=0;
    for(;i<connection.size();++i)
       if(connection[i]==nullptr){ connection[i]=(m_ptcpServer->nextPendingConnection()); break;}

if(i==connection.size()-1 && connection[i]!=nullptr){
    connection.push_back(m_ptcpServer->nextPendingConnection());
    ++i;
}

    connect(connection[i], SIGNAL(disconnected()), connection[i], SLOT(deleteLater()));
    connect(connection[i], SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    //добавляем соединение в болк connections нашей программы
    if(con->is_ready)
       con->update_table();

/////////////////////////////////алгоритм распределения пакетов между клиентами////////////////////
if(!pack.empty() && db->count_pack_of_client!=pack.size()-1){
    if(con_size()!=0){
sendToClient(connection[i], pack[db->count_pack_of_client]);
++db->count_pack_of_client;}
}
//////////////////////////////////////////////////////////////////////////////////////////////////

}

//когда приходят данные от клиента
void MainWindow::slotReadClient()
{
QTcpSocket* pClientSocket = (QTcpSocket*)sender();
QDataStream in(pClientSocket); //создаем обьект для получения данных
in.setVersion(QDataStream::Qt_5_3);
QTime time;
int num_of_pack = 0;
int flag = 0;
int pass = 0;
//////////////////выводим ip///////////////////////////
QString Name, strMes;
QString s = pClientSocket->peerAddress().toString();
auto parts = s.split("::ffff:");
QString IpAddres = (parts[parts.size()-1]);
///////////////////////////////////////////////////////
for (;;) {
    if(!m_nNextBlockSize) {
        if(pClientSocket->bytesAvailable() < sizeof(quint16)) break;
        in>>m_nNextBlockSize;
    }
    if(pClientSocket->bytesAvailable() < m_nNextBlockSize) break;
in >> flag >> time;

 if(flag==mas_status::DELIVER){
     in >>num_of_pack>>pass;   
     Packet a(num_of_pack,time,0,0,0,"");
     auto it = std::find(pack.begin(), pack.end(), a);
     if(it!=pack.end()){
         ui->textEdit->appendPlainText(QTime::currentTime().toString()+": Client with ip: " +
                              IpAddres +
                              "make a request of the packet on deliver: " + QString::number(it->number_of_packet));
         //отсылаем qr-коде стелажа который нужно доставить и qr-код куда нужно доставить (int del_qr)        
         mp->send_to_pack(it->number_of_table,chek_pass_on_map(pass));
     }
     strMes = time.toString() + " Client has sent number of the packet - " + QString::number(num_of_pack);

//вызов функций отправки роботов за товарами из пакета
 }

 if(flag==mas_status::ACCEPT){
//удаление пакета и изменения в БД
     in >>num_of_pack>>pass;
     Packet a(num_of_pack,time,0,0,0,"");
     auto it = std::find(pack.begin(), pack.end(), a);
     if(it!=pack.end()){
     ui->textEdit->appendPlainText(QTime::currentTime().toString()+": Client with ip: " +
                          IpAddres +
                          "confirmed receipt of the package: " + QString::number(it->number_of_packet));
     db->del_packet_from_db(*it);
     delpacket(*it);
     mp->get_out_table(it->number_of_table, chek_pass_on_map(pass));//отвезти стелаж
     //если имеется выслать следующий пакет этому клиенту
     if(!pack.empty() && db->count_pack_of_client!=pack.size()-1){
     sendToClient(pClientSocket, pack[db->count_pack_of_client]);
     ++db->count_pack_of_client;
     }     }
 strMes = time.toString() + " Client has sent number of the packet - " + QString::number(num_of_pack);
 }

 if(flag==mas_status::CONNECT){
    in>>Name>>pass;
    pClientSocket->setObjectName(Name);    
      if(!(pass==pass::FIRST||pass==pass::SECOND||pass==pass::THIRD)){
         for(size_t k=0;k<connection.size();++k)
             if(pClientSocket==connection[k]) dell_soc(k);
         con->update_table();
      }

    strMes = time.toString() + ": Client "+ Name +" and Ip: " + IpAddres + " connected";
    auto it = std::find(connection.begin(),connection.end(), pClientSocket);
    if(it!=connection.end()){
    it.operator*()->setObjectName(Name);
    con->update_table();
    }
 } 
 m_nNextBlockSize=0;
}
ui->textEdit->appendPlainText(strMes);
}

//функция отправки пакета на подтверждение клиенту
void MainWindow::sendToClient(QTcpSocket *pSocket, Packet& n_p)
{
QByteArray arrBlock;
QDataStream out(&arrBlock, QIODevice::WriteOnly); //связываем обьект потока данных массивом данных байт
//записываем в массив данные
out.setVersion(QDataStream::Qt_5_3);
//////////////////////////////////////////////////////////////////////////////
out<<quint16(0)<<QTime::currentTime()<<n_p.artic<<n_p.number_of_table<<n_p.number_of_packet
  <<n_p.number_of_product<<n_p.name_of_product; //отправляем текущее время и строку
//////////////////////////////////////////////////////////////////////////////
out.device()->seek(0);
out<<quint16(arrBlock.size() - sizeof(quint16));
//передаем данные через сокет клиенту
pSocket->write(arrBlock);//фукция отправки пакета байтов клиенту
QString s = pSocket->peerAddress().toString();
auto parts = s.split("::ffff:");
QString IpAddres = (parts[parts.size()-1]);
ui->textEdit->appendPlainText(QTime::currentTime().toString()+": Packet with number: "
                     +QString::number(n_p.number_of_packet)
                     +" send to client with addres: "
                     +IpAddres);
}
/////////////////////////////////////////для распределения пакетов///////////////////////////
size_t MainWindow::con_size()
{
    size_t sz=0;
    for(auto it:connection) if(it!=nullptr) ++sz;
    return sz;
}

QTcpSocket *MainWindow::get_random_con()
{
    auto beg = connection.begin();
    while(*(beg+rand()%connection.size())==nullptr)beg = connection.begin();
    return *beg;
}

int MainWindow::chek_pass_on_map(int pass)
{
    switch (pass/1000) {
    case(1): return client_qr::CLIENT1;
    case(2): return client_qr::CLIENT2;
    case(3): return client_qr::CLIENT3;
    default: return 0;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_DataBase_clicked()
{   
    db->show();
}

void MainWindow::on_Connection_clicked()
{
    con->show();
    connect(con, SIGNAL(dis_soc(int)), this, SLOT(dell_soc(int)));
}

void MainWindow::on_Map_clicked()
{
    mp->show();
}

void MainWindow::on_GetPacket_clicked()
{
    srand(time(nullptr));
    pack.push_back(db->get_packet());
    ui->textEdit->appendPlainText(QTime::currentTime().toString()+ ": Packet with number: "+
                         QString::number(pack[pack.size()-1].number_of_packet) + " received from DB");
    addpacket(pack[pack.size()-1]);
///////////////////////////////////алгоритм распределения пакетов между клиентами////////////////////

        if(con_size()!=0){
            if(!pack.empty() && db->count_pack_of_client!=pack.size()-1){
    sendToClient(get_random_con(), pack[db->count_pack_of_client]);
       ++db->count_pack_of_client;}
        }
/////////////////////////////////////////////////////////////////////////////////////////////////////
}
//функци отключения клиента от сервера по запросу из окна connection
void MainWindow::dell_soc(int row)
{
    if(connection.at(row)!=nullptr){
        QString s = connection.at(row)->peerAddress().toString();
        auto parts = s.split("::ffff:");
        QString IpAddres = (parts[parts.size()-1]);

//        ui->textEdit->appendPlainText(QTime::currentTime().toString()+": the server disabled the client with the address: " +
//                             IpAddres);
        QString text = (QTime::currentTime().toString()+": the server disabled the client with the address: " +
                                                    IpAddres);
           QTextCharFormat tf = ui->textEdit->currentCharFormat();
           tf.setForeground(QBrush(Qt::red));
           ui->textEdit->setCurrentCharFormat(tf); ui->textEdit->appendPlainText(text);
           tf.setForeground(QBrush(Qt::black)); ui->textEdit->setCurrentCharFormat(tf);


       connection.at(row)->disconnectFromHost();

       connection.at(row)=nullptr;

       con->del_soc_from_model(row);}
}



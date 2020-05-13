#include "myclient.h"
#include "ui_myclient.h"
#include <QDialog>
#include <QMessageBox>

MyClient::MyClient( QWidget *parent)
    : QWidget(parent), m_nNextBlockSize(0),is_connected(false), ui(new Ui::MyClient)
{
    m_pTcpSocket = new QTcpSocket(this);

    ui->setupUi(this);
    on_Connect_Serv_clicked();

/////////////////////////////////////////////////////////////////////////////////////////////
    model = new QStandardItemModel(this);
    model2 = new QStandardItemModel(this);

    QStringList horizontalHeader1;
        horizontalHeader1.append("Время получения пакета");
        horizontalHeader1.append("              Номер пакета              ");
    QStringList verticalHeader1;

    QStringList horizontalHeader2;
            horizontalHeader2.append("Qr-code стелажа");
            horizontalHeader2.append("Количества товара");
            horizontalHeader2.append("Артикул товара");
            horizontalHeader2.append("Наименование товара");
    QStringList verticalHeader2;

model->setHorizontalHeaderLabels(horizontalHeader1);
model->setVerticalHeaderLabels(verticalHeader1);

model2->setHorizontalHeaderLabels(horizontalHeader2);
model2->setVerticalHeaderLabels(verticalHeader2);


ui->tableView->setModel(model);
ui->tableView->resizeRowsToContents();
ui->tableView->resizeColumnsToContents();

ui->tableView_2->setModel(model2);
ui->tableView_2->resizeRowsToContents();
ui->tableView_2->resizeColumnsToContents();
ui->label_4->setText("Welcome to app!");
}

MyClient::~MyClient()
{
    delete ui;
}
//добавление переданного пакета в таблицу
void MyClient::addpacket(Packet &n_p)
{
    QStandardItem *item;
    int index = model->rowCount();
    item = new QStandardItem(n_p.time_of_accept.toString());
    model->setItem(index,0,item);
    item = new QStandardItem(QString::fromStdString(std::to_string(n_p.number_of_packet)));
    model->setItem(index,1,item);
}
//если удалось подключится к серверу
void MyClient::slotConnected()
{         
    is_connected=true;
    ui->label_4->setText("Connected with server: "+last_serv_ip);
}

void MyClient::disconnect()
{
    if(is_connected==true){
QMessageBox* mes = new QMessageBox(this);
mes->setText("Disconected from host, try connect again ...");
ui->label_4->setText("Disconnect about server: "+last_serv_ip);
mes->show();
on_Connect_Serv_clicked();
is_connected=false;
    }
}

//прием байтов от сервера
void MyClient::slotReadyRead()
{
QDataStream in(m_pTcpSocket); //обьект для приема байт
in.setVersion(QDataStream::Qt_5_3);
Packet new_pack;
for (;;) {
    if(!m_nNextBlockSize) {
        if(m_pTcpSocket->bytesAvailable() < sizeof(quint16)) break;
        in>>m_nNextBlockSize;
    }
    if(m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) break;
///////////////////////////////////////////////////получаем из сервера сразу в обьекты    
    in>>new_pack.time_of_accept>>new_pack.artic>>new_pack.number_of_table>>
            new_pack.number_of_packet>>new_pack.number_of_product>>new_pack.name_of_product;
    pack.push_back(new_pack);
    addpacket(pack[pack.size()-1]);
///////////////////////////////////////////////////
    m_nNextBlockSize=0;
         }
    ui->label_4->setText("Get packet with number: "+QString::number(new_pack.number_of_packet));
}
//если нужно отправить данные серверу
void MyClient::slotSendToServer(int flag)
{ 
QByteArray arrBlock;
QDataStream out(&arrBlock,QIODevice::WriteOnly);//связываем обьект потока данных массивом данных байт
out.setVersion(QDataStream::Qt_5_3);
//записываем необходимые и введенные данные от клиента
if((flag==mas_status::ACCEPT) || (flag==mas_status::DELIVER)){
    if(flag!=2)
   out<<quint16(0)<<flag<<QTime::currentTime()<<pack[ui->tableView->currentIndex().row()].number_of_packet<<pass;
}
    else if(flag==mas_status::CONNECT)
     out<<quint16(0)<<flag<<QTime::currentTime()<<Name<<pass;

out.device()->seek(0);
out<<quint16(arrBlock.size() - sizeof(quint16));
//передаем записанные данные на сервер
m_pTcpSocket->write(arrBlock);
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{
QString strError = "Error: " + (err==QAbstractSocket::HostNotFoundError?
                   "The host was not found." : err==QAbstractSocket::RemoteHostClosedError?
                   "The remote host is closed." :err==QAbstractSocket::ConnectionRefusedError?
                   "The connection was refused.":QString(m_pTcpSocket->errorString())
                                );
qDebug()<<strError;
}
//при нажатии на строку в таблицу пакетов выводит ее содержимое в таблицу товаров
void MyClient::on_tableView_clicked(const QModelIndex &index)
{
    ui->label_4->setText("Selected index in table: "+QString::number((index.row()+1)*(index.column()+1)));
QStandardItem *item;
//тут можно поставить цикл который будет выводить все товары в таблицу
item = new QStandardItem(QString::number(pack[index.row()].number_of_table));
model2->setItem(0,0,item);
item = new QStandardItem(QString::number(pack[index.row()].number_of_product));
model2->setItem(0,1,item);
item = new QStandardItem(QString::number(pack[index.row()].artic));
model2->setItem(0,2,item);
item = new QStandardItem(pack[index.row()].name_of_product);
model2->setItem(0,3,item);
}

//сделать чтоб нельзя было нажать accept без deliver
//deliver
void MyClient::on_pushButton_2_clicked()
{
    ui->label_4->setText("Deliver last pressed button");
    slotSendToServer(mas_status::DELIVER);
}
//accept
void MyClient::on_pushButton_clicked()
{
    ui->label_4->setText("Accept last pressed button");
    slotSendToServer(mas_status::ACCEPT);
    //удаление пакета
   model->removeRow(ui->tableView->currentIndex().row());
}

void MyClient::open_serv()
{
    is_connected=false;
    OpenServ* wnd = new OpenServ(this);
    wnd->show();

    connect(wnd, SIGNAL(Selection(QString, QString, QString)), this, SLOT(makerequest(QString, QString, QString)));
}

void MyClient::makerequest(const QString &ip, const QString &name, const QString &pass)
{
 m_pTcpSocket->connectToHost(ip, 2323);

 connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
 connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
 connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
         this, SLOT(slotError(QAbstractSocket::SocketError)));
 connect(m_pTcpSocket, SIGNAL(disconnected()),SLOT(disconnect()));
Name = name; ui->label_3->setText("User name: "+Name);
this->pass = pass.toInt(); this->last_serv_ip = ip;
slotSendToServer(mas_status::CONNECT);
}

void MyClient::on_Connect_Serv_clicked()
{
    open_serv();
}

#include "map.h"
#include "ui_map.h"
#include <QErrorMessage>

Map::Map(QSqlDatabase *db,std::vector<QR>* qr_map,QWidget *parent) :
   db(db), qr_map(qr_map), QMainWindow(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    Robots_server = new QTcpServer(this); // создаем сервер при открытии программы
    if(!Robots_server->listen(QHostAddress::Any, 1323)){
      qDebug()<<"Error, close server!";
        Robots_server->close();
        return;
    }
   QList<QHostAddress> addressList = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, addressList )
           qDebug()<<("yet another address: %s" +  address.toString());

    connect(Robots_server, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
//////////////////////////////////////////////////////////////////////////////////////////////
    QString table_name = db->tables()[0];

    model = new QSqlTableModel(this, *db); //запускаем нашу таблицу и показываем ее через внутриний класс таблиц
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); //OnFieldChange по-умолчанию
    model->setTable(table_name); //выбираем таблицу в базе
    model->select();//делаем выборку всей таблицы
    setup_map(); //настройка карты склада

    ui->RobotsBox->clear();
    srand(time(nullptr));
}
//в деструкторе мы должны освободить все выделенные под пути роботов данные
Map::~Map()
{
    delete ui;
    for (size_t i=0;i<robo_path.size();++i)
       delete robo_path[i];

}
//инициализация карты по qr_map из главного окна
void Map::setup_map()
{
    model_robots = new QStandardItemModel(this);

    QStandardItem *item;
    item = new QStandardItem("NULL");
    for(size_t i=0;i<6;++i)
        for(size_t j=0;j<12;++j){
            if(qr_map->at(i*12+j).numb_post==0){
    item = new QStandardItem(QString::number(qr_map->at(i*12+j).qr_state)+"\n");
    model_robots->setItem(i,j,item);}
            else{
                item = new QStandardItem(QString::number(qr_map->at(i*12+j).qr_state) +
                                         " \nPost: " +
                                         QString::number(qr_map->at(i*12+j).numb_post));
                item->setBackground(QColor(Qt::yellow));
                model_robots->setItem(i,j,item);
            }
        }
    ui->tableView->setModel(model_robots);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    for(int i=0;i<6;++i)
    ui->tableView->setRowHeight(i,60);
    for(int i=0;i<12;++i)
    ui->tableView->setColumnWidth(i,60);
}
//алгоритм построения пути Дима!
void Map::build_way(int qr_code, int sel_robot)
{
    //...//пока просто буду переносить робота сразу на нужную qr метку
    if(robo_path[sel_robot]->size()==0) robo_path[sel_robot]->push_back(qr_code);
    for(size_t i=0;i<robo_path[sel_robot]->size();++i)
    robo_path[sel_robot]->at(i)=qr_code; //записываем в выбранный массив номера qr_меток так как роботу нужно будет перемещатья по ним до нужного qr_code//
    //...
}
//выбор подходящего робота в зависимости от его расположения и удаленности от места назначения (qr_code)
int Map::selected_robot(int qr_code)
{
    //Доделать выбор робота
    //Для начала проверям какие роботы свободны
    //потом проверяем какте из свободных роботов ближе
    //и возвращаем его номер
    return (rand() % (connection.size()));//выбираем рандомного робота (если робот 1 возвращает 0)
}
//получаем от клиента qr-код стола и qr-код места куда нужно доставить стол
void Map::send_to_pack(int numb_table, int qr_deliv)
{
    if(!qr_deliv){ QErrorMessage().showMessage("Error on send_to_pack to clietn"); return; }
    int table_qr=0; // находим где стол
for(size_t i=0;i<qr_map->size();++i)
    if(qr_map->at(i).qr_post==numb_table) table_qr = qr_map->at(i).qr_state;

     int s_rob = selected_robot(qr_deliv);//выбираем робота и получаем его номер в списах
     auto it = std::find(rob_cl.begin(),rob_cl.end(),qMakePair(s_rob,0));
     if(it!=rob_cl.end())it->second=qr_deliv;

     sendToClient(connection[s_rob],table_qr,s_rob,false);
//отправляем робота за столом (false ЕДИНСТВЕННОЕ место где это нужно, именно для того чтоб робот привез стелаж обратно клиенту)
}
//вызывается в slotReadClient если робот передал нам флаг захвата flag1 как true и нужно вести стелаж кленту
void Map::send_to_client(int s_rob, int qr_deliv)
{
    sendToClient(connection[s_rob],qr_deliv,s_rob,true); //если true то выключаем обратную подачу стелажа до клиента
}

//получаем от клиента qr-код стола и qr-код места где сейчас стол
void Map::get_out_table(int numb_table, int qr_deliv)
{
    qDebug()<<qr_deliv;
    if(!qr_deliv){ QErrorMessage().showMessage("Error on send_to_pack to clietn"); return; }
    int table_qr=0; // находим где стол
for(size_t i=0;i<qr_map->size();++i)
    if(qr_map->at(i).qr_post==numb_table) {table_qr = qr_map->at(i).qr_state; break;}

   if(table_qr!=0)
for(size_t i=0;i<robo_path.size();++i)
    if(robo_path[i]->at(robo_path[i]->size()-1)==qr_deliv){
        sendToClient(connection[i],table_qr,i,true);
        break;
    }
}
//при подкючении нового робота добавляем его в список подключений и создаем для него индивидуальный массив qr-кодов (путь)
void Map::slotNewConnection()
{
    connection.push_back(Robots_server->nextPendingConnection()); //при подключении нового робота записываем его в список подключенных
    qDebug()<<"New robot: " + connection.at(connection.size()-1)->peerAddress().toString();
    ui->RobotsBox->addItem("Robot " + QString::number(connection.size()-1)); //присваиваем ему номер соответвующий сокету робота в массиве соединений

    std::vector<int>* new_rob = new std::vector<int>; //создаем новый путь для каждого робота
    robo_path.push_back(new_rob);
    current_pos.push_back(0); //добавляем новую позицию в массив положений

    rob_cl.push_back(qMakePair(connection.size()-1,0));

    connect(connection[connection.size()-1], SIGNAL(disconnected()), connection[connection.size()-1], SLOT(deleteLater())); //не заботимся корректности данных
    //мы полагаем, что роботы будут подсоеденены к серверу пока он вулючен, а при новом включении создаются новые данные (условность)
    connect(connection[connection.size()-1], SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}
//считывем qr-код текущего положения робота
void Map::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    qDebug()<<"new robot mass " + pClientSocket->peerAddress().toString();
    QDataStream in(pClientSocket); //создаем обьект для получения данных
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
     in>>m_nNextBlockSize;

     QTime time;
     int current_qr_code; //передает свое местоположение
     bool flag1/*,flag2*/; //уведомление что робот взял стелаж

     in >> time >> current_qr_code >>flag1/*>>flag2*/;
     qDebug()<<"robot send!";
//с помощью этого цикла узнаем где находится конкретный робот (i - Номер робота, current_qr_code - местоположение) и записываем его в списко местоположений роботов
     int rob_n=0;
     for(size_t i=0;i<connection.size();++i)
       if(pClientSocket==connection.at(i)){
           //замазываем старое положение найденого робота белым (если робот отсылает начальное местополоежение то проскакиваем проверку)
           if(current_pos[i]!=0){
           int row=(current_pos[i]/10000)-1;
           int cul=((current_pos[i]-(12*row))%100)-1;
//ищем в нашей карте что должно быть под конекретной точкой
           QStandardItem *item;
           if(qr_map->at(row*12+cul).numb_post==0)item = new QStandardItem(QString::number(qr_map->at(row*12+cul).qr_state)+"\n");
           else item = new QStandardItem(QString::number(qr_map->at(row*12+cul).qr_state) +" \nPost: " + QString::number(qr_map->at(row*12+cul).numb_post));

           if((qr_map->at(row*12+cul).qr_post)==0)
               item->setBackground(QColor(Qt::white));
           else item->setBackground(QColor(Qt::yellow));
           model_robots->setItem(row,cul,item);
//устанавливаем размеры таблицы чтоб красиво было
           for(int i=0;i<6;++i)
           ui->tableView->setRowHeight(i,60);
           for(int i=0;i<12;++i)
           ui->tableView->setColumnWidth(i,60);
//
           }
           //новое положение ставим в красный
           current_pos[i]=current_qr_code;
          int row=(current_qr_code/10000)-1;
          int  cul=(current_qr_code-(12*row))%100-1;
           QStandardItem *it;
           if(qr_map->at(row*12+cul).numb_post==0)it = new QStandardItem(QString::number(qr_map->at(row*12+cul).qr_state) + "\n");
           else it = new QStandardItem(QString::number(qr_map->at(row*12+cul).qr_state) + " \nPost: " + QString::number(qr_map->at(row*12+cul).numb_post));
           it->setBackground(QColor(Qt::red));

           model_robots->setItem(row,cul,it);
//устанавливаем размеры таблицы чтоб красиво было
           for(int i=0;i<6;++i)
           ui->tableView->setRowHeight(i,60);
           for(int i=0;i<12;++i)
           ui->tableView->setColumnWidth(i,60);
//

           qDebug()<<"robots cum to: " + QString::number(current_qr_code);
           rob_n=i;
            break;
       }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     QString strMes = time.toString() + " Robot: " + pClientSocket->peerAddress().toString() + " has sent - " + QString::number(current_qr_code);
     qDebug()<<strMes;
     m_nNextBlockSize=0;
if(flag1) //если робот передал нам флаг на возврат обратно, значит он взял стелаж и нужно отправить его к клиенту
    send_to_client(rob_n,rob_cl[rob_n].second);    // то отослать к месту выдачи
     break;
}
}
//передаем путь(массив Qr-кодов) выбранному роботу до указанной qr_code метки
void Map::sendToClient(QTcpSocket *pSocket, int qr_code, int sel_robot, bool without_flag=true)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly); //связываем обьект потока данных массивом данных байт
    //записываем в массив данные
    out.setVersion(QDataStream::Qt_5_3);
    //////////////////////////////////////////////////////////////////////////////
    build_way(qr_code, sel_robot); //строим путь для выбранного робота до qr_code
    out<<quint16(0)<<without_flag<<robo_path[sel_robot]->size(); //отсылаем размер массива qr-меток для создания пути
    for(size_t i=0;i<robo_path[sel_robot]->size();++i)
        out<<robo_path[sel_robot]->at(i); //отсылаем сам массив (путь)

    //////////////////////////////////////////////////////////////////////////////
    out.device()->seek(0);
    out<<quint16(arrBlock.size() - sizeof(quint16));
    //передаем данные через сокет клиенту-роботу
    pSocket->write(arrBlock);//фукция отправки пакета байтов клиенту
}


//отправляем выбранного робота к указанной точке
void Map::on_Send_clicked()
{
   int qr_end_way = qr_map->at( ui->tableView->currentIndex().row()*12 + ui->tableView->currentIndex().column()).qr_state; //находим qr код на месте куда нажал клиент на карте
   int s_r = ui->RobotsBox->currentIndex();
// int s_r = selected_robot(qr_end_way);//выбираем робота и получаем его номер в списах
   sendToClient(connection.at(s_r),qr_end_way, s_r, true); //передаем сокет выбранного робота, qr_метку куда ему ехать и номер робота в списках путей и подключений
    //если true то не включаем обратную подачу стелажа до клиента
}
//отправляем выбранного робота на зарядку
void Map::on_SendToCharging_clicked()
{
    int s_r = ui->RobotsBox->currentIndex();
    //проверяем все пути роботов на предмет того не отправлены ли они к станциям зарядки и выбираем свободную станцию
    bool crg1=true,crg2=true,crg3=true;
    for(size_t i=0;i<robo_path.size();++i){ //у всех путей
        if((robo_path.at(i)->at(robo_path.at(i)->size()-1))==charging_qr::CHARG1) //проверяем последний qr-код пути на предмет того не станция ли это зарядки
            crg1=false;
        if((robo_path.at(i)->at(robo_path.at(i)->size()-1))==charging_qr::CHARG2)
            crg2=false;
        if((robo_path.at(i)->at(robo_path.at(i)->size()-1))==charging_qr::CHARG3)
            crg3=false;
    }
     //если передаем true то не включаем обратную подачу стелажа до клиента (с.м. readtoclient)
      if(crg1){ sendToClient(connection.at(s_r),charging_qr::CHARG1, s_r,true);   return;} //отпраялем к первой из свободных по списку
       if(crg2) { sendToClient(connection.at(s_r),charging_qr::CHARG2, s_r,true);   return;}
        if(crg3) { sendToClient(connection.at(s_r),charging_qr::CHARG3, s_r,true);   return;}


}

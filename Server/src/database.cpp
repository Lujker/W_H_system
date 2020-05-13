#include "database.h"
#include "ui_database.h"

DataBase::DataBase(QSqlDatabase  *db, std::vector<QR>* qr_map, QWidget *parent) :
    QMainWindow(parent), db(db),
    ui(new Ui::DataBase), qr_map(qr_map)
{
    ui->setupUi(this);
//    db = QSqlDatabase::addDatabase("QSQLITE"); //подключаем драйвер для sqlite
//    db.setDatabaseName("warehouse.db"); //открываем базу по умолчанию
//    if(!db.open()){ //если не открылось выводим причину ошибки
//        qDebug()<< db.lastError().text();
//        return;
//    }
//    else {
//        qDebug()<<"Complete";
//    }

    QString table_name = db->tables()[0];

    model = new QSqlTableModel(this, *db); //запускаем нашу таблицу и показываем ее через внутриний класс таблиц
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); //OnFieldChange по-умолчанию
    model->setTable(table_name); //выбираем таблицу в базе
    model->select();//делаем выборку всей таблицы
    ui->tableView->setModel(model);

    sort_model = new QSqlTableModel(this, *db);
    sort_model->setEditStrategy(QSqlTableModel::OnManualSubmit); //OnFieldChange по-умолчанию
    sort_model->setTable(table_name); //выбираем таблицу в базе
    sort_model->select();//делаем выборку всей таблицы
    ui->tableView_2->setModel(sort_model);

        ui->comboBox->clear();
        for(QString &table_name:db->tables())
            ui->comboBox->addItem(table_name);
        srand(time(nullptr));
}

DataBase::~DataBase()
{
    delete ui;
}

void DataBase::on_add_row_box_clicked()
{
    qDebug()<<"inserting row"<< model->insertRow(model->rowCount());
}

void DataBase::on_reselect_box_clicked()
{
    db->close();
    if(!db->open()){ //если не открылось выводим причину ошибки
        qDebug()<< db->lastError().text();
        return;
    }
    else {
        qDebug()<<"Complete\n";
    }
    model->select();
    sort_model->select();
}

void DataBase::on_revert_box_clicked()
{
    model->revertAll(); //отменить изменения
    qDebug()<<"reverting model";
}

void DataBase::on_delete_row_box_clicked()
{
    int selectedRow = ui->tableView->currentIndex().row();
              if(selectedRow>=0){
               qDebug()<<"deleting row:"<<model->removeRow(selectedRow);
}
              else{
              qDebug()<<"no row selected";
}
}

void DataBase::on_submit_box_clicked()
{
    //проверям чтоб все вписанные номера толов соответствовали qr-номерам стелажей
    for(int x=0;x<=model->rowCount();++x){
    QModelIndex ind = model->index(x,0);
    QR ser_qr;
    ser_qr.numb_post=ind.data().toInt();
    auto it = std::find(qr_map->begin(),qr_map->end(),ser_qr);
//    qDebug()<<"finding..." + QString::number(it->numb_post) + "==" + QString::number(it->qr_post);
    if(it!=qr_map->end()){
        model->setData(model->index(x,1),it->qr_post);
    }
    else qDebug()<<"it==end";
    }
    qDebug()<<"submiting model";
    model->submitAll();
}


void DataBase::on_dialog_search_clicked()
{
    QString arg = ui->lineEdit->text();
    sort_model->setFilter(arg);
    sort_model->select();
}

void DataBase::on_request_clicked()
{
   SQL_code* wnd = new SQL_code(this);
   wnd->show();

   connect(wnd, SIGNAL(Selection(QString)), this, SLOT(makerequest(QString)));
}

void DataBase::makerequest(const QString &req)
{
    query = new QSqlQuery(*db);
    if(query->exec(req))
    model->select();
    else {
    qDebug()<< query->lastError().text();
    }
}

void DataBase::on_pushButton_clicked()
{
    path = QFileDialog::getOpenFileName(this,"Open file", "", "Database (*db)");

    db->setDatabaseName(path); //открываем базу по умолчанию


    if(!db->open()){ //если не открылось выводим причину ошибки
        qDebug()<< db->lastError().text();
        return;
    }
    else {
        qDebug()<<"Complete\n";
    }
    ui->comboBox->clear();
    for(QString &table_name:db->tables())
        ui->comboBox->addItem(table_name);


    model = new QSqlTableModel(this, *db); //запускаем нашу таблицу и показываем ее через внутриний класс таблиц
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); //OnFieldChange по-умолчанию
    model->setTable(ui->comboBox->currentText()); //выбираем таблицу в базе
    model->select();//делаем выборку всей таблицы
    ui->tableView->setModel(model);

    sort_model = new QSqlTableModel(this, *db);
    sort_model->setEditStrategy(QSqlTableModel::OnManualSubmit); //OnFieldChange по-умолчанию
    sort_model->setTable(ui->comboBox->currentText()); //выбираем таблицу в базе
    sort_model->select();//делаем выборку всей таблицы
    ui->tableView_2->setModel(sort_model);
}

void DataBase::on_comboBox_currentIndexChanged(const QString &arg1)
{
    model->setTable(arg1); //выбираем таблицу в базе
    model->select();//делаем выборку всей таблицы
    ui->tableView->setModel(model);

    sort_model->setTable(arg1); //выбираем таблицу в базе
    sort_model->select();//делаем выборку всей таблицы
    ui->tableView_2->setModel(sort_model);
}


void DataBase::on_Boutton_clear_clicked()
{
    ui->lineEdit->clear();
}
//собираем пакет для выполнения (эмоляция запросов с головного сервера)
Packet DataBase::get_packet()
{
Packet new_pack;
unsigned i =rand() % model->rowCount();
new_pack.number_of_packet = rand()%99999;
new_pack.time_of_accept = QTime::currentTime();

new_pack.number_of_table = model->index(i,1).data().toInt();
new_pack.artic = model->index(i,2).data().toInt();
new_pack.name_of_product = model->index(i,3).data().toString();
new_pack.number_of_product = model->index(i,4).data().toInt();

new_pack.number_of_product = rand()%new_pack.number_of_product;
return new_pack;
}
//пакет получен клиентом, удаляем из БД //не удаляет из базы данных почемут
bool DataBase::del_packet_from_db(Packet &pc)
{
    //находим по артикулу
    unsigned result_numb=0;
    for(int i=0;i<=model->columnCount();i++){
    for(int x=0;x<=model->rowCount();x++){
    QModelIndex ind = model->index(x,i);
    if(ind.data().toInt()==pc.artic){
        qDebug()<<ind.data().toInt()<< "  "<< pc.artic << " Найденное поле";
    ind = model->index(x,i+2);
    qDebug()<< model->index(x,i+2).data().toInt()<<" Начальное значение ";
    result_numb = ind.data().toInt() - pc.number_of_product;
    model->setData(ind,result_numb);
    qDebug()<<(model->index(x+2,i).data().toInt())<<" Измененной значение ";
    model->submitAll();
    return true;
    }
    }}
    //////////////////////////////делаем запрос на изменение в БД/////////////////////////////////////////////////////////////
//    QString req = "UPDATE " + db.tables()[0] + " SET number = " + QString::number(result_numb) + " WHERE vendor_code = " + QString::number(pc.artic);
//    if(query->exec(req))
//    model->select();
//    else {
//    qDebug()<< query->lastError().text();
//    return false;
//    }
    return false;
}

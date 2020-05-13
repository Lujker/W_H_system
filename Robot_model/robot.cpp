#include "robot.h"
#include "ui_robot.h"

Robot::Robot(QWidget *parent)
    : QMainWindow(parent),
     m_nNextBlockSize(0), in_path(false), current_qr(10001), ui(new Ui::Robot)
{
    ui->setupUi(this);
    ui->textEdit->append(QTime::currentTime().toString() + ": setup robot");
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost("localhost", 1323);
    ui->textEdit->append(QTime::currentTime().toString() + ": connected to host");

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
}

Robot::~Robot()
{
    delete ui;
}

void Robot::slotReadyRead()
{
    while(in_path) { } //пока робот находится в пути не принимаем команды (защита от некоректного управления)
ui->textEdit->append(QTime::currentTime().toString() + ": reed mass...");
    QDataStream in(m_pTcpSocket); //обьект для приема байт
    in.setVersion(QDataStream::Qt_5_3);
    bool without_flag = true;//если считали флаг как false то включаем функцию обратного отпраления клиенту стелажа при его захвате
    for (;;) {
        if(!m_nNextBlockSize) {
            if(m_pTcpSocket->bytesAvailable() < sizeof(quint16)) break;
            in>>m_nNextBlockSize;
        }
        if(m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) break;
    ///////////////////////////////////////////////////получаем из сервера сразу в обьекты(разобартся как получать пакет)
    size_t size,new_qr;

        in>>without_flag>>size; //принимаем размер массива пути
        if(path.size()==0)
        for(size_t i=0;i<size;++i){
            in>>new_qr;
            path.push_back(new_qr); //принимаем массив qr_кодов для построения пути
        }
        else{
            size_t i=0;
            for(;i<size && i<path.size();++i){
                in>>new_qr;
                path.at(i)=new_qr;
            }
            if(i<size){
                for(;i<size;++i){
                    in>>new_qr;
                    path.push_back(new_qr); //если нехватило места ватсавляем push'ом отсавшиеся
                }
            }

                if(i<path.size()){ //если осталиьс лишнее элементы с прошлого раза
                    path.resize(size); //оставляем size первых элементов массива все остальные удаляем
                }
        }
    ///////////////////////////////////////////////////
        m_nNextBlockSize=0;
    }
    come_to_path(without_flag); //когда получили весь путь вызываем функцию отправки робота по пути
}


void Robot::slotError(QAbstractSocket::SocketError err)
{
    QString strError = ": Error: " + (err==QAbstractSocket::HostNotFoundError?
                       "The host was not found." : err==QAbstractSocket::RemoteHostClosedError?
                       "The remote host is closed." :err==QAbstractSocket::ConnectionRefusedError?
                       "The connection was refused.":QString(m_pTcpSocket->errorString())
                                    );
    qDebug()<<strError;
     ui->textEdit->append(QTime::currentTime().toString() + strError);
}

void Robot::slotSendToServer(int qr_code,bool without_flag=true)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);//связываем обьект потока данных массивом данных байт
    out.setVersion(QDataStream::Qt_5_3);
    //записываем необходимые и введенные данные от клиента
    bool flag1=false; //флаг прибытия к клиенту
    if(!without_flag){ //ТОТ САМЫЙ ФЛАГ ВОЗАРВЩЕНИЯ СТЕЛАЖА КЛИЕНТУ (ЕСЛИ TRUE ТО ОТКЛЮЧАЕМ ЭТУ ФУНКЦИЮ)
    if(path.size()!=0){//ОН УСТАНАВЛИВАЕТСЯ КОГДА ВЗЯТ СТЕЛАЖ И ЕСЛИ СЧИТАТЬ ЕГО КАК TRUE В СЕРВЕРЕ БУДЕТ ВЫЗВАНА ФУНКЦИЯ ОТПРАВКИ РОБОТА КЛИЕНТУ
    flag1=(current_qr==path[path.size()-1])&&(current_qr!=client_qr::CLIENT1)&&(current_qr!=client_qr::CLIENT2)&&(current_qr!=client_qr::CLIENT3); //выставляем флаг взятия стелажа
    }
    }
    ui->textEdit->append(QTime::currentTime().toString()+ ": Robot send to server: current QR-code:" + QString::number(qr_code) + flag1);
    qDebug()<<quint16(0)<<QTime::currentTime()<<qr_code<<flag1;
    out<<quint16(0)<<QTime::currentTime()<<qr_code<<flag1;
    if(out.device()->seek(0)) qDebug()<<"seek true";
    out<<quint16(arrBlock.size() - sizeof(quint16));
    //передаем записанные данные на сервер
    m_pTcpSocket->write(arrBlock);
}

void Robot::slotConnected()
{
    qDebug()<<"Received the connected() signal";
     ui->textEdit->append(QTime::currentTime().toString()+": Received the connected() signal");
    slotSendToServer(current_qr);
}

void Robot::come_to_path(bool without_flag)
{
    qDebug() << "come to path ...";
     ui->textEdit->append(QTime::currentTime().toString()+": come to path ...");
    in_path=true;
    for(size_t i =0; i<path.size();++i){
        current_qr=path[i];
        std::this_thread::sleep_for(std::chrono::seconds(2));
        ui->textEdit->append(QTime::currentTime().toString()+": current QR-code is: " + QString::number(current_qr));
        slotSendToServer(current_qr, without_flag);
    }
    in_path=false;
}


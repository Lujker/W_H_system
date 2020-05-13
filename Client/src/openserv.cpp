#include "openserv.h"
#include "ui_openserv.h"


OpenServ::OpenServ(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenServ)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(on_buttonBox_clicked()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(close()));
}

OpenServ::~OpenServ()
{
    delete ui;
}

void OpenServ::on_buttonBox_clicked()
{
    emit Selection(ui->lineEdit->text(), ui->lineEdit_3->text() ,ui->lineEdit_2->text()); //генерируем сигнал c ip
    close();
}

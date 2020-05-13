#include "sql_code.h"
#include "ui_sql_code.h"
#include <QPushButton>

SQL_code::SQL_code(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SQL_code)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(on_buttonBox_clicked()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(close()));
}

SQL_code::~SQL_code()
{
    delete ui;
}

void SQL_code::on_buttonBox_clicked()
{
    emit Selection(ui->lineEdit->text()); //генерируем сигнал
    close();
}

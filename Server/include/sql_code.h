#ifndef SQL_CODE_H
#define SQL_CODE_H

#include <QDialog>
#include <QSqlTableModel>
#include <QAbstractButton>

namespace Ui {
class SQL_code;
}

class SQL_code : public QDialog
{
    Q_OBJECT

public:
    explicit SQL_code(QWidget *parent = nullptr);
    ~SQL_code();

private:
    Ui::SQL_code *ui;

signals:
    void Selection(const QString &str);

private slots:
    void on_buttonBox_clicked();
};

#endif // SQL_CODE_H

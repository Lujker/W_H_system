#ifndef OPENSERV_H
#define OPENSERV_H

#include <QDialog>
#include <QButtonGroup>
#include <QPushButton>

namespace Ui {
class OpenServ;
}

class OpenServ : public QDialog
{
    Q_OBJECT

public:
    explicit OpenServ(QWidget *parent = nullptr);
    ~OpenServ();

signals:
    void Selection(const QString &ip, const QString &name, const QString &pass);

private slots:
    void on_buttonBox_clicked();

private:
    Ui::OpenServ *ui;
};

#endif // OPENSERV_H

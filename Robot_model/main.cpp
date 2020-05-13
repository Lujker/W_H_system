#include "robot.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Robot w;
    w.show();
    return a.exec();
}

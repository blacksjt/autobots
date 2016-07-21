#include "autobots_sina.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    autobots_toutiao w;
    w.show();
    return a.exec();
}

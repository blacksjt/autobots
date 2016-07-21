#include "autobots_weibo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    autobots_weibo w;
    w.show();
    return a.exec();
}

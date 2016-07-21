#include "autobots_weibo_dianzan.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    autobots_weibo_dianzan w;
    w.show();
    return a.exec();
}

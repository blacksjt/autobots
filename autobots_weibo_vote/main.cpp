#include "autobots_weibo_vote.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    autobots_weibo_vote w;
    w.show();
    return a.exec();
}

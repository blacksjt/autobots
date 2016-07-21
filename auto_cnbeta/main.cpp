#include "auto_cnbeta.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_cnbeta w;
    w.show();
    return a.exec();
}

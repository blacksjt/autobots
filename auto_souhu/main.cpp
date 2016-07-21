#include "auto_sohu.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_sohu w;
    w.show();
    return a.exec();
}

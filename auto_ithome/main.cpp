#include "auto_ithome.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_ithome w;
    w.show();
    return a.exec();
}

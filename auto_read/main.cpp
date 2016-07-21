#include "auto_read.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_smzdm w;
    w.show();
    return a.exec();
}

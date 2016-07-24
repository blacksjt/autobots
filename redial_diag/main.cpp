#include "redial_diag.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    redial_diag w;
    w.show();
    return a.exec();
}

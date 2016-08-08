#include "getcomments.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GetComments w;
    w.show();
    return a.exec();
}

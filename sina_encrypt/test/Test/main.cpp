
#include <QtCore/QCoreApplication>
#include "image_process.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ProcessImage();
    return a.exec();
}

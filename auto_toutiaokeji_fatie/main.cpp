#include "auto_toutiaokeji_fatie.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto_toutiaokeji_fatie w;
    w.show();
    return a.exec();
}

#include "auto_ithome_fatie.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	auto_ithome_fatie w;
    w.show();
    return a.exec();
}

#include "auto_dirvers_mobile.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	auto_dirvers_fatie w;
	w.show();
	return a.exec();
}

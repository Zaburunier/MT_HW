#include "mainwindow.h"
#include "menu.h"

#include <QApplication>
#include <QIcon>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	MainWindow* w = new MainWindow();

	QIcon windowIcon("d:/qt/projects/hermite/images/spline_icon.png");
	w->setWindowIcon(windowIcon);
	w->setWindowTitle("Эрмитовы сплайны");


	w->show();
    return a.exec();

	delete w;
}

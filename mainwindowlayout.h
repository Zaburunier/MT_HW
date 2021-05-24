#include <QApplication>
#include <QBoxLayout>

#ifndef MAINWINDOWLAYOUT_H
#define MAINWINDOWLAYOUT_H


class MainWindowLayout
{
	private:
		QBoxLayout* layout;

	public:
		MainWindowLayout(QWidget *parent = nullptr);
		~MainWindowLayout();

		QBoxLayout GetLayout();


};

#endif // MAINWINDOWLAYOUT_H

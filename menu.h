#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#ifndef MENU_H
#define MENU_H


class Menu: public QWidget
{
	public:
		QMenuBar* menuBar;

		Menu(QWidget *parent = nullptr);
		~Menu();

	private:
		static void ShowAuthor();
};

#endif // MENU_H

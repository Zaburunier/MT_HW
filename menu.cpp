#include "menu.h"
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QIcon>
#include <QWidget>
#include "mainwindow.h"

Menu::Menu(QWidget *parent) : QWidget() {
	// Список опций
	QAction *resetAction = new QAction("Reset", this);
	QAction *authorAction = new QAction("Author", this);
	QAction *exitAction = new QAction("Leave", this);

	QMenuBar *mainWindowMenu = new QMenuBar();
	// Формируем меню
	mainWindowMenu->addAction(resetAction);
	mainWindowMenu->addAction(authorAction);
	mainWindowMenu->addSeparator();
	mainWindowMenu->addAction(exitAction);

	connect(resetAction, &QAction::triggered, static_cast<MainWindow*>(parent), &MainWindow::ResetHermite);
	connect(exitAction, &QAction::triggered, qApp, QApplication::quit);
	connect(authorAction, &QAction::triggered, qApp, Menu::ShowAuthor);

	this->menuBar = mainWindowMenu;
}

void Menu::ShowAuthor() {
	QMessageBox authorMsg;
	authorMsg.setText("РТ5-61Б, Забурунов Леонид, 2021г.");
	authorMsg.setWindowTitle("Автор программы");
	authorMsg.setIcon(QMessageBox::Information);
	authorMsg.exec();
}

Menu::~Menu() {
	delete this->menuBar;
}

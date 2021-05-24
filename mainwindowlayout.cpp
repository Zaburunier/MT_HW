#include "mainwindowlayout.h"

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

MainWindowLayout::MainWindowLayout(QWidget *parent) {
	// Создаём горизонтально ориентированную разметку
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, parent);

	// Создаём необходимые виджеты


	this->layout = layout;

}

MainWindowLayout::~MainWindowLayout() {
	// Уничтожаем объекты
	delete this->layout;
}




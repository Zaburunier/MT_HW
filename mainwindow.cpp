#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include <QBoxLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->menu = new Menu(this);
	this->setMenuBar(this->menu->menuBar);
	this->graph = ui->plot;

	this->derivativeEditor = new DerivativeEditor(ui->derivativeSlider, ui->sliderValueText, ui->spinBox);
	SetupHermite();
}


// Настройка рабочей области отрисовки + инициализация обработчика для работы со сплайном
void MainWindow::SetupHermite() {
	/* В QCustomPlot есть возможность добавлять на виджет несколько графиков и взаимодействовать с каждым по отдельности.
	   В связи с этим будем использовать два графика:
		1. Верхний слой, где будут располагаться контрольные точки, с которыми можно взаимодействовать;
		2. Нижний слой, где будет отрисовываться сплайн на основе заданных контр. точек.
	   Соответственно, graph(0) отвечает за отрисовку точек, graph(1) - за кривую.*/
	this->graph->addGraph(graph->xAxis, graph->yAxis);
	this->graph->addGraph(graph->xAxis, graph->yAxis);

	// Параметры отрисовки узловых точек
	this->graph->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,
														   QColor(255, 170, 0),
														   QColor(255, 170, 0),
														   14));
	// Помимо самих точек, будем показывать штрихпунктирной линией кусочно-линейную функцию на основе узловых точек
	QPen pen0;
	pen0.setStyle(Qt::PenStyle::DashDotLine);
	pen0.setColor(QColor(0, 36, 61));
	pen0.setWidth(2);
	this->graph->graph(1)->setPen(pen0);

	// Параметры отрисовки выбранной узловой точки (к оранжевой точке добавится красная обводка)
	QPen decPen;
	decPen.setWidth(14);
	decPen.setBrush(Qt::GlobalColor::red);
	QCPSelectionDecorator* decorator = new QCPSelectionDecorator();
	decorator->setPen(decPen);
	decorator->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,
											   QPen(QBrush(Qt::GlobalColor::red), 3),
											   QBrush(QColor(255, 170, 0)),
											   18), QCPScatterStyle::ScatterProperty::spAll);
	decorator->setBrush(Qt::GlobalColor::red);
	this->graph->graph(1)->setSelectionDecorator(decorator);

	// Параметры выбора элементов на верхнем слое
	this->graph->setSelectionTolerance(50);
	this->graph->setInteractions(QCP::iSelectPlottables | QCP::iRangeZoom);
	this->graph->graph(1)->setSelectable(QCP::SelectionType::stSingleData);

	// То же самое для графика, где будет рисоваться сплайн
	this->graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssNone)); // нужны только линии
	this->graph->graph(0)->setSelectable(QCP::SelectionType::stNone);

	QPen pen1;
	pen1.setStyle(Qt::PenStyle::SolidLine);
	pen1.setCapStyle(Qt::PenCapStyle::RoundCap);
	pen1.setWidth(4);
	pen1.setBrush(QColor(0, 61, 20));
	this->graph->graph(0)->setPen(pen1);

	// Инициализация обработчика построения сплайна
	this->hermite = new HermiteBuilder(this);
}


QCustomPlot* MainWindow::GetPlotWidget() {
	return this->graph;
}


DerivativeEditor* MainWindow::GetDerEditor() {
	return this->derivativeEditor;
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->type() == QKeyEvent::KeyPress && event->key() == Qt::Key_Plus) {
		emit AddPointKeyPressed();
	} else if (event->type() == QKeyEvent::KeyPress && event->key() == Qt::Key_Minus) {
		emit RemovePointKeyPressed();
	}
}


MainWindow::~MainWindow()
{
    delete ui;
}


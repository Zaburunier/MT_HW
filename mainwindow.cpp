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

	this->dered = new DerivativeEditor(this, ui->derivativeSlider, ui->sliderValueText, ui->spinBox);
	SetupHermite();
}


void MainWindow::SetupHermite() {
	this->graph->addGraph(graph->xAxis, graph->yAxis);
	this->graph->addGraph(graph->xAxis, graph->yAxis);

	// Параметры отрисовки второго графика, на котором размещаем узловые точки
	this->graph->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,
														   QColor(255, 170, 0),
														   QColor(255, 170, 0),
														   14));
	QPen pen0;
	pen0.setStyle(Qt::PenStyle::DashDotLine);
	pen0.setColor(QColor(0, 36, 61));
	pen0.setWidth(2);
	this->graph->graph(1)->setPen(pen0);

	// Параметры отрисовки выбранной точки
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

	// Параметры выбора элементов
	this->graph->setSelectionTolerance(50);
	this->graph->setInteractions(QCP::iSelectPlottables | QCP::iRangeZoom);
	this->graph->graph(1)->setSelectable(QCP::SelectionType::stSingleData);

	// То же самое для первого графика, где будет рисоваться сплайн
	this->graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssNone)); // нужны только линии
	this->graph->graph(0)->setSelectable(QCP::SelectionType::stNone);

	QPen pen1;
	pen1.setStyle(Qt::PenStyle::SolidLine);
	pen1.setCapStyle(Qt::PenCapStyle::RoundCap);
	pen1.setWidth(4);
	pen1.setBrush(QColor(0, 61, 20));
	this->graph->graph(0)->setPen(pen1);

	this->hermite = new HermiteBuilder(this);
}


QCustomPlot* MainWindow::GetPlotWidget() {
	return this->graph;
}


HermiteBuilder* MainWindow::GetHermiteBuilder() {
	return this->hermite;
}


Ui::MainWindow* MainWindow::GetUI() {
	return this->ui;
}


DerivativeEditor* MainWindow::GetDerEditor() {
	return this->dered;
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->type() == QKeyEvent::KeyPress && event->key() == Qt::Key_Plus) {
		emit AddPointKeyPressed();
	} else if (event->type() == QKeyEvent::KeyPress && event->key() == Qt::Key_Minus) {
		emit RemovePointKeyPressed();
	}
}


void MainWindow::ResetHermite() {
	//SetupHermite();
}


MainWindow::~MainWindow()
{
    delete ui;
}


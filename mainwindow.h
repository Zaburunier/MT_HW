#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "menu.h"
#include "qcustomplot.h"
#include "hermitebuilder.h"
#include "derivativeeditor.h"
#include <QMainWindow>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class HermiteBuilder;
class DerivativeEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();

		QCustomPlot* GetPlotWidget();
		//HermiteBuilder* GetHermiteBuilder();
		DerivativeEditor* GetDerEditor();


	private:
		Ui::MainWindow *ui;
		Menu *menu;
		QCustomPlot *graph;
		HermiteBuilder *hermite;
		DerivativeEditor *derivativeEditor;

		void SetupHermite();


	protected:
		void keyPressEvent(QKeyEvent *event);


	signals:
		void AddPointKeyPressed();
		void RemovePointKeyPressed();
};
#endif // MAINWINDOW_H

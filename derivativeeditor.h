#ifndef DERIVATIVEEDITOR_H
#define DERIVATIVEEDITOR_H

#include <QWidget>
#include <QLabel>
#include "mainwindow.h"
#include "hermitebuilder.h"
#include <QSlider>
#include <QSpinBox>

class HermitePoint;

class DerivativeEditor : public QWidget
{
		Q_OBJECT
	public:
		explicit DerivativeEditor(QWidget *parent = nullptr);
		DerivativeEditor(MainWindow *mw, QSlider *slider, QLabel *valueLabel, QSpinBox* spinBox);

		void SetValuesListPointer(QList<HermitePoint> *pointer);

	signals:
		void sliverDoubleValueChanged(int index, double newDoubleValue);

	private:
		QSlider *slider;
		QLabel *valueLabel;
		QSpinBox *indexSpinBox;
		QList<HermitePoint> *values;

	public slots:
		void OnSpinBoxValueChanged(int index);
		void OnSliderValueChanged(int newValue);
		void OnPointsResize(int newSize);
		void OnSelectedPointChange(int newIndex);


};

#endif // DERIVATIVEEDITOR_H

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
		DerivativeEditor(QSlider *slider, QLabel *valueLabel, QSpinBox* spinBox);

		void SetValuesListPointer(QList<HermitePoint> *pointer);


	signals:
		void sliverDoubleValueChanged(int index, double newDoubleValue);


	private:
		// Ползунок для изменения значения производной
		QSlider *slider;
		// Текстовое поле для отображения текущего значения производной
		QLabel *valueLabel;
		// Поле для выбора точки, значение которой меняем
		QSpinBox *indexSpinBox;
		// Указатель на список, с которым работаем
		QList<HermitePoint> *values;


	public slots:
		void OnSpinBoxValueChanged(int index);
		void OnSliderValueChanged(int newValue);
		void OnPointsResize(int newSize);
		void OnSelectedPointChange(int newIndex);


};

#endif // DERIVATIVEEDITOR_H

#include "derivativeeditor.h"
#include "mainwindow.h"
#include <QDebug>

class HermitePoint;

DerivativeEditor::DerivativeEditor(QWidget *parent) : QWidget(parent)
{
}


DerivativeEditor::DerivativeEditor(QSlider *slider, QLabel *valueLabel, QSpinBox *spinBox) {
	this->valueLabel = valueLabel;
	this->slider = slider;
	this->indexSpinBox = spinBox;
	// При движении слайдера нужно обновлять значения в текстовом поле, а так же в изначальном массиве данных
	connect(slider, &QSlider::valueChanged, this, &DerivativeEditor::OnSliderValueChanged);
	// При смене точки, для которой меняем производную, нужно подгрузить записанное значение
	connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &DerivativeEditor::OnSpinBoxValueChanged);
}


// Чтобы знать, откуда брать значения и куда их класть
void DerivativeEditor::SetValuesListPointer(QList<HermitePoint> *pointer) {
	this->values = pointer;
}


// SpinBox отвечает за выбор текущей рабочей точки
// При изменении точки загружаем в ползунок уже имеющееся значение из массива
void DerivativeEditor::OnSpinBoxValueChanged(int index) {
	qDebug() << "Spin box index changed:" << index << "; expected value = " << values->at(index).derivative;
	// Слайдер работает только с целочисленными значениями, а нам нужно иметь возможность задавать действительные числа
	// Поэтому у слайдера диапазон [-100; 100], но в реальных данных всё делится на 10
	slider->setValue(static_cast<int>(values->at(index).derivative) * 10);
	valueLabel->setText(QString::number(values->at(index).derivative));
}


// Slider отвечает за значение производной в выбранной рабочей точке
// При изменении значения загружаем новое в текстовое поле и в массив
void DerivativeEditor::OnSliderValueChanged(int newValue) {
	qDebug() << "Slider value changed: " << newValue;
	// Слайдер работает только с целочисленными значениями, а нам нужно иметь возможность задавать действительные числа
	// Поэтому у слайдера диапазон [-100; 100], но в реальных данных всё делится на 10
	double newDoubleValue = static_cast<double>(newValue) / 10;
	valueLabel->setText(QString::number(newDoubleValue));
	emit sliverDoubleValueChanged(indexSpinBox->value(), newDoubleValue);
}


void DerivativeEditor::OnPointsResize(int newSize) {
	qDebug() << "Points amount changed: " << newSize;
	if (newSize == 0) {
		// Если точек пока нет, то работать не с чем
		this->indexSpinBox->setMinimum(-1);
		this->indexSpinBox->setMaximum(-1);
	} else {
		this->indexSpinBox->setMinimum(0);
		this->indexSpinBox->setMaximum(newSize - 1);
	}
}

void DerivativeEditor::OnSelectedPointChange(int index) {
	indexSpinBox->setValue(index);
}

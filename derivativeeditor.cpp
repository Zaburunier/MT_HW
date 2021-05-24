#include "derivativeeditor.h"
#include "mainwindow.h"
#include <QDebug>

class HermitePoint;

DerivativeEditor::DerivativeEditor(QWidget *parent) : QWidget(parent)
{
}


DerivativeEditor::DerivativeEditor(MainWindow *mw, QSlider *slider, QLabel *valueLabel, QSpinBox *spinBox) {
	this->valueLabel = valueLabel;
	this->slider = slider;
	this->indexSpinBox = spinBox;
	// При движении слайдера нужно обновлять значения в текстовом поле, а так же в изначальном массиве данных
	connect(slider, &QSlider::valueChanged, this, &DerivativeEditor::OnSliderValueChanged);
	// При смене точки, для которой меняем производную, нужно подгрузить записанное значение
	connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &DerivativeEditor::OnSpinBoxValueChanged);
}


void DerivativeEditor::SetValuesListPointer(QList<HermitePoint> *pointer) {
	this->values = pointer;
}


void DerivativeEditor::OnSpinBoxValueChanged(int index) {
	qDebug() << "Spin box index changed:" << index << "; expected value = " << values->at(index).derivative;
	slider->setValue(static_cast<int>(values->at(index).derivative) * 10);
	valueLabel->setText(QString::number(values->at(index).derivative));
}


void DerivativeEditor::OnSliderValueChanged(int newValue) {
	qDebug() << "Slider value changed: " << newValue;
	double newDoubleValue = static_cast<double>(newValue) / 10;
	valueLabel->setText(QString::number(newDoubleValue));
	emit sliverDoubleValueChanged(indexSpinBox->value(), newDoubleValue);
}


void DerivativeEditor::OnPointsResize(int newSize) {
	qDebug() << "Points amount changed: " << newSize;
	if (newSize == 0) {
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




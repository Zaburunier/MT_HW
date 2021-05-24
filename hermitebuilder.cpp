#include "hermitebuilder.h"
#include <QDebug>
#include <QSharedPointer>
#include "mainwindow.h"
#include "alglib/interpolation.h"


HermiteBuilder::HermiteBuilder()
{
	this->values = new QList<HermitePoint>();
	this->selectedPointIndex = -1;
	this->offsetBeforeDragging = new QPointF(0.f, 0.f);
	this->isDragging = false;
}

HermiteBuilder::HermiteBuilder(MainWindow *mw) : HermiteBuilder() {
	auto qcp = mw->GetPlotWidget();
	this->basePointsGraph = qcp->graph(1);
	this->splineGraph = qcp->graph(0);
	// Подвязываем события мыши для интерактивного вз-я
	connect(qcp, &QCustomPlot::mousePress, this, &HermiteBuilder::OnMouseClick);
	connect(qcp, &QCustomPlot::mouseMove, this, &HermiteBuilder::OnMouseMove);
	connect(qcp, &QCustomPlot::mouseRelease, this, &HermiteBuilder::OnMouseRelease);

	// Подвязываем события клавиатуры для регулирования числа точек
	connect(mw, &MainWindow::AddPointKeyPressed, this, &HermiteBuilder::OnAddingNewPoint);
	connect(mw, &MainWindow::RemovePointKeyPressed, this, &HermiteBuilder::OnRemovingPoint);

	// Привязываем изменение выбранной точки графика
	connect(qcp, &QCustomPlot::selectionChangedByUser, this, &HermiteBuilder::OnChangingSelection);

	// Подвязываем редактор производных для корректной работы
	connect(mw->GetDerEditor(), &DerivativeEditor::sliverDoubleValueChanged, this, &HermiteBuilder::OnDerivativeChanged);
	connect(this, &HermiteBuilder::SelectedPointChanged, mw->GetDerEditor(), &DerivativeEditor::OnSelectedPointChange);
	connect(this, &HermiteBuilder::PointsSizeChanged, mw->GetDerEditor(), &DerivativeEditor::OnPointsResize);

	mw->GetDerEditor()->SetValuesListPointer(this->values);

}

void HermiteBuilder::AddPoint(QPointF *newPoint = nullptr) {
	if (this->values->size() >= MAX_LENGTH) {
		qDebug() << "Нельзя добавить точку, достигнут максимальный размер!";
		return;
	}

	if (newPoint == nullptr) {
		if (values->size() > 0) {
			if (this->selectedPointIndex == -1) {
				newPoint = new QPointF(values->last().point + QPointF(1.f, 1.f));
			} else { // есть выбранная точка
				if (this->selectedPointIndex == values->size() - 1) { // Добавляем точку после крайней
					// Делаем последнюю точку серединой между предпоследней и новой
					newPoint = new QPointF(values->last().point + (values->last().point - values->at(values->size() - 2).point));
				} else if (this->selectedPointIndex > -1) { // Добавляем точку между выбранной и следующей
					newPoint = new QPointF(0.5 * (values->at(selectedPointIndex).point + values->at(selectedPointIndex + 1).point));
				}
			}
		} else { // первая точка на добавление
			newPoint = new QPointF(0.f, 0.f);
		}
	}

	values->append(HermitePoint(*newPoint, 0));
	std::sort(values->begin(), values->end(), ComparePoints_X);
	Redraw(true);

	emit PointsSizeChanged(values->size());
}


void HermiteBuilder::RemovePoint(int index) {
	if (values->size() == 0) {
		qDebug() << "Не осталось точек для удаления!";
		return;
	} else if (values->size() <= index || index < 0) {
		qDebug() << "Неверный индекс для удаления!";
		return;
	}

	values->removeAt(index);
	// Если не обнулять выбор, то пойдут ошибки IndexOutOfRangeException (см. OnMouseMove())
	this->selectedPointIndex = -1;
	basePointsGraph->parentPlot()->deselectAll();
	Redraw(true);

	emit PointsSizeChanged(values->size());
}


bool HermiteBuilder::ComparePoints_X(HermitePoint point1, HermitePoint point2) {
	return point1.point.x() < point2.point.x();
}


bool HermiteBuilder::ComparePoints_Y(HermitePoint point1, HermitePoint point2) {
	return point1.point.y() < point2.point.y();
}


void HermiteBuilder::Redraw(bool newRangeApplied) {
	if (values->isEmpty() == true) return;

	// Обновление данных
	QCPGraphDataContainer* container = new QCPGraphDataContainer();
	for (auto value : *values) {
		container->add(QCPGraphData(value.point.x(), value.point.y()));
	}
	this->basePointsGraph->setData(QSharedPointer<QCPGraphDataContainer>(container));

	// Настройка границ
	if (newRangeApplied == true) {
		// Смотрим на текущую область, занимаемую графиком
		double xMin = std::min_element(values->begin(), values->end(), ComparePoints_X)->point.x(),
			 xMax = std::max_element(values->begin(), values->end(), ComparePoints_X)->point.x(),
			 yMin = std::min_element(values->begin(), values->end(), ComparePoints_Y)->point.y(),
			 yMax = std::max_element(values->begin(), values->end(), ComparePoints_Y)->point.y();
		//qDebug() << xMin << xMax << "; " << yMin << yMax;
		// Смотрим на то, какие на виджете границы сейчас
		QCPRange xRange = basePointsGraph->parentPlot()->xAxis->range(),
				 yRange = basePointsGraph->parentPlot()->yAxis->range();
		double xThreshold =  0.05 * (xRange.upper - xRange.lower),
			   yThreshold = 0.05 * (yRange.upper - yRange.lower);
		//qDebug() << xRange << yRange;
		// Если график слишком близко к границе или выходит за неё, то меняем границы
		if (xMin - xRange.lower < xThreshold|| yMin - yRange.lower < yThreshold ||
		xRange.upper - xMax < xThreshold || yRange.upper - yMax < yThreshold) {
			basePointsGraph->parentPlot()->xAxis->setRange(xMin - 2 * xThreshold, xMax + 2 * xThreshold);
			basePointsGraph->parentPlot()->yAxis->setRange(yMin - 2 * yThreshold, yMax + 2 * yThreshold);
		}
	}

	basePointsGraph->parentPlot()->replot();
	DrawSplines();
}


void HermiteBuilder::DrawSplines() {
	if (values->size() < 2) return;

	// Загрузка модели сплайна
	alglib::spline1dinterpolant *spline = new alglib::spline1dinterpolant();
	alglib::real_1d_array xArray, yArray, dArray;
	xArray.setlength(values->size());
	yArray.setlength(values->size());
	dArray.setlength(values->size());
	for (int i = 0; i < values->size(); i = i + 1) {
		xArray[i] = values->at(i).point.x();
		yArray[i] = values->at(i).point.y();
		dArray[i] = values->at(i).derivative;
	}
	alglib::spline1dbuildhermite(xArray, yArray, dArray, *spline);

	// Отрисовка
	QCPGraphDataContainer* container = new QCPGraphDataContainer();
	for (int i = 0; i < values->size() - 1; i = i + 1) { // Для каждой пары точек
		QPointF point1 = values->at(i).point,
				point2 = values->at(i + 1).point;
		int amountOfSteps =  10 + 20 * std::sqrt(std::pow(point2.x() - point1.x(), 2) + std::pow(point2.y() - point1.y(), 2));
		qDebug() << amountOfSteps;
		for (int j = 0; j <= amountOfSteps; j = j + 1) { // Для каждой промежуточной точки
			double x = point1.x() + static_cast<float>(j) / amountOfSteps * (point2.x() - point1.x()), // линейно интерполируем по X
				   y = alglib::spline1dcalc(*spline, x); // получаем результат по Y
			container->add(QCPGraphData(x, y));
		}
	}
	this->splineGraph->setData(QSharedPointer<QCPGraphDataContainer>(container));
	splineGraph->parentPlot()->replot();

}


void HermiteBuilder::OnMouseClick(QMouseEvent *event) {
	if (this->selectedPointIndex == -1) return;
	// Запомним вектор от мыши до точки, чтобы при переносе соблюдать его
	QPointF vector;
	this->basePointsGraph->pixelsToCoords(event->localPos(), vector.rx(), vector.ry());
	this->offsetBeforeDragging = new QPointF(vector - this->values->at(selectedPointIndex).point);
	//qDebug() << "Offset = " << *offsetBeforeDragging;

	this->isDragging = true;

}


void HermiteBuilder::OnMouseMove(QMouseEvent *event) {
	if (this->selectedPointIndex == -1 || this->isDragging == false) return;
	// Новое положение точки = положение мыши - offset
	QPointF vector;
	basePointsGraph->pixelsToCoords(event->localPos(), vector.rx(), vector.ry());
	double temp_d = values->at(selectedPointIndex).derivative;

	this->values->replace(this->selectedPointIndex, HermitePoint(vector - *(this->offsetBeforeDragging), temp_d));
	Redraw(false);
}


void HermiteBuilder::OnMouseRelease(QMouseEvent *event) {
	if (this->selectedPointIndex == -1) return;

	this->offsetBeforeDragging = new QPointF(0.f, 0.f);

	this->isDragging = false;
	Redraw(true);
}


void HermiteBuilder::OnAddingNewPoint() {
	//qDebug() << "Добавляем точку";
	AddPoint();
}


void HermiteBuilder::OnRemovingPoint() {
	if (selectedPointIndex >= 0) {
		//qDebug() << "Удаляем точку";
		RemovePoint(selectedPointIndex);
	} else {
		//qDebug() << "Не выбрана точка для удаления";
	}
}


void HermiteBuilder::OnDerivativeChanged(int index, double newValue) {
	if (index == -1) return;
	qDebug() << "Derivative changed: " << newValue << " for index " << index;
	QPointF temp_p(values->at(index).point);
	this->values->replace(index, HermitePoint(temp_p, newValue));
	Redraw(false);
}


void HermiteBuilder::OnChangingSelection() {
	QCPDataSelection selected = this->basePointsGraph->selection();

	if (selected.dataRangeCount() == 0) { 	// Ставим -1 для корректной работы в других методах
		this->selectedPointIndex = -1;
		//qDebug() << selectedPointIndex;
	} else {
		auto point = basePointsGraph->data()->at(selected.dataRange(0).begin());
		selectedPointIndex = HermitePoint::IndexOf(this->values, QPointF(point->key, point->value));
		//qDebug() << selected.dataRange(0).begin() << selected.dataRange(0).end() << point->key << point->value;
	}

	emit SelectedPointChanged(selectedPointIndex);
}


HermiteBuilder::~HermiteBuilder() {
	delete values;
	delete offsetBeforeDragging;
	delete basePointsGraph;
	delete splineGraph;
}

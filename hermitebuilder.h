#ifndef HERMITEBUILDER_H
#define HERMITEBUILDER_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QPointF>
#include <QMouseEvent>

class MainWindow;
class QCustomPlot;
class QCPGraph;


// Структура данных для хранения всех исходных данных сплайна
class HermitePoint {
	public:
		QPointF point;
		double derivative;

		HermitePoint(QPointF p, double d) {
			point = p;
			derivative = d;
		}

		static int IndexOfPoint(QList<HermitePoint> *list, QPointF point) {
			for (int i = 0; i <  list->size(); i = i + 1) {
				if (list->at(i).point == point) return i;
			}

			return -1;
		}
};


class HermiteBuilder : public QWidget
{
	Q_OBJECT


	public:
		HermiteBuilder();
		HermiteBuilder(MainWindow *mw);

		void AddPoint(QPointF *newPoint);
		void RemovePoint(int index);

		~HermiteBuilder();


	private:
		const int MAX_LENGTH = 10;
		//const int SPLINE_STEPS = 40;
		QList<HermitePoint> *values;
		// Ссылки на оба слоя рабочей области
		QCPGraph* basePointsGraph;
		QCPGraph* splineGraph;
		// Индекс выбранной точки (чтобы знать, что в массиве менять)
		int selectedPointIndex;
		// Смещение от мыши до курсора на начало перемещения (для сохранения)
		QPointF *offsetBeforeDragging;
		// Флаг активности процесса перемещения
		bool isDragging;

		// Критерии сортировки
		static bool ComparePoints_X(HermitePoint point1, HermitePoint point2);
		static bool ComparePoints_Y(HermitePoint point1, HermitePoint point2);
		// Перерисовка графиков после любых событий, изменяющих значения узловых точек
		void Redraw(bool newRangeApplied);
		// Построение сплайна на основе заданных значений
		void DrawSplines();


	signals:
		void PointsSizeChanged(int newSize);
		void SelectedPointChanged(int newIndex);


	private slots:
		void OnMouseClick(QMouseEvent *event);
		void OnMouseMove(QMouseEvent *event);
		void OnMouseRelease(QMouseEvent *event);
		void OnAddingNewPoint();
		void OnRemovingPoint();
		void OnChangingSelection();


	public slots:
		void OnDerivativeChanged(int index, double value);
};

#endif // HERMITEBUILDER_H

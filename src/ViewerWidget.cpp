#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) {
		data[x + y * img->width()] = color.rgb();
	}
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

void ViewerWidget::KresliKruznicu(QPoint* P1, QPoint* P2, QColor* Color)
{
	int deltaX = P2->x() - P1->x();
	int deltaY = P2->y() - P1->y();
	int r = round(sqrt(deltaX * deltaX + deltaY * deltaY));
	int p = 1 - r;
	int surX = 0;
	int surY = r;
	int dvaX = 3;
	int dvaY = 2*r + 2;

	do {
		setPixel(P1->x() + surX, P1->y() + surY, *Color);
		setPixel(P1->x() + surX, P1->y() - surY, *Color);
		setPixel(P1->x() - surX, P1->y() + surY, *Color);
		setPixel(P1->x() - surX, P1->y() - surY, *Color);

		setPixel(P1->x() + surY, P1->y() + surX, *Color);
		setPixel(P1->x() + surY, P1->y() - surX, *Color);
		setPixel(P1->x() - surY, P1->y() + surX, *Color);
		setPixel(P1->x() - surY, P1->y() - surX, *Color);

		if (p > 0) {
			p = p - dvaY;
			surY--;
			dvaY -= 2;
		}
		p = p + dvaX;
		dvaX += 2;
		surX++;
	} while (surX <= surY);

	update();
}
void ViewerWidget::KresliUseckuDDA(QPoint* P1, QPoint* P2, QColor* Color)
{
	int deltaX = P2->x() - P1->x();
	int deltaY = P2->y() - P1->y();
	float m = (float)deltaY / deltaX;

	if ((m >= 1.0) || (m <= -1.0)) {
		//riadiaca os Y
		if (P1->y() > P2->y()) {
			PointSwap(*P1, *P2);		//y1 ma byt mensie ako y2
		}
		float surX = P1->x();
		int surY = P1->y();
		do {
			setPixel(round(surX), surY, *Color);
			surY++;
			surX += 1/m;
		} while (surY < P2->y());
	}
	else {
		//riadiaca os X
		if (P1->x() > P2->x()) {
			PointSwap(*P1, *P2);		//x1 ma byt mensie ako x2
		}
		int surX = P1->x();
		float surY = P1->y();
		do {
			setPixel(surX, round(surY), *Color);
			surX++;
			surY += m;
		} while (surX < P2->x());
	}
	update();
}
void ViewerWidget::KresliUseckuBresen(QPoint* P1, QPoint* P2, QColor* Color)
{
	int deltaX = P2->x() - P1->x();
	int deltaY = P2->y() - P1->y();
	float m = (float)deltaY / deltaX;
	int k1, k2, p, surX, surY;
	
	//riadiaca os Y
	
	if (m > 1.0) {

		if (P1->y() > P2->y()) {
			PointSwap(*P1, *P2);		//y1 ma byt mensie ako y2
			deltaX = P2->x() - P1->x();
			deltaY = P2->y() - P1->y();
		}

		k1 = 2 * deltaX;
		k2 = 2 * deltaX - 2 * deltaY;
		p = 2 * deltaX - deltaY;
		surX = P1->x();
		surY = P1->y();

		do {
			setPixel(surX, surY, *Color);
			surY++;
			if (p > 0) {
				surX++;
				p += k2;
			}
			else {
				p += k1;
			}
		} while (surY < P2->y());
	}

	else if (m < -1.0) {

		if (P1->y() > P2->y()) {
			PointSwap(*P1, *P2);		//y1 ma byt mensie ako y2
			deltaX = P2->x() - P1->x();
			deltaY = P2->y() - P1->y();
		}

		k1 = 2 * deltaX;
		k2 = 2 * deltaX + 2 * deltaY;
		p = 2 * deltaX + deltaY;
		surX = P1->x();
		surY = P1->y();

		do {
			setPixel(surX, surY, *Color);
			surY++;
			if (p < 0) {
				surX--;
				p += k2;
			}
			else {
				p += k1;
			}
		} while (surY < P2->y());
	}

	//riadiaca os X

	else if ((m >= 0.0) && (m <= 1.0)) {

		if (P1->x() > P2->x()) {
			PointSwap(*P1, *P2);		//x1 ma byt mensie ako x2
			deltaX = P2->x() - P1->x();
			deltaY = P2->y() - P1->y();
		}

		k1 = 2 * deltaY;
		k2 = 2 * deltaY - 2 * deltaX;
		p = 2 * deltaY - deltaX;
		surX = P1->x();
		surY = P1->y();

		do {
			setPixel(surX, surY, *Color);
			surX++;
			if (p > 0) {
				surY++;
				p += k2;
			}
			else {
				p += k1;
			}
		} while (surX < P2->x());
	}
	else if((m >= -1.0) && (m < 0.0)) {

		if (P1->x() > P2->x()) {
			PointSwap(*P1, *P2);		//x1 ma byt mensie ako x2
			deltaX = P2->x() - P1->x();
			deltaY = P2->y() - P1->y();

		}

		k1 = 2 * deltaY;
		k2 = 2 * deltaY + 2 * deltaX;
		p = 2 * deltaY + deltaX;
		surX = P1->x();
		surY = P1->y();

		do {
			setPixel(surX, surY, *Color);
			surX++;
			if (p < 0) {
				surY--;
				p += k2;
			}
			else {
				p += k1;
			}
		} while (surX < P2->x());
	}

	update();

}

void ViewerWidget::PointSwap(QPoint& P1, QPoint& P2)
{
	QPoint P3 = P1;
	P1 = P2;
	P2 = P3;
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

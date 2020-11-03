#include "CanvasImage.h"
#include <qpainter.h>
//
// CanvasImage
//
int CanvasImage::RTTI()
{
	return 1050;
}

CanvasImage::CanvasImage(const QImage& a, QCanvas * canvas) : QCanvasPolygonalItem(canvas), image(a) {
	image.detach();
}

CanvasImage::~CanvasImage() {
	hide();
}

int CanvasImage::rtti() const
{
	return CanvasImage::RTTI();
}

QPointArray CanvasImage::areaPoints () const
{	
	QRect bound(QPoint((int) x(), (int) y()),  image.size());
	QPointArray Result(4);

	Result.setPoint(0, bound.left(),  bound.top()    );
	Result.setPoint(1, bound.right(), bound.top()    );
	Result.setPoint(2, bound.right(), bound.bottom() );
	Result.setPoint(3, bound.left(),  bound.bottom() );

	return Result;
}

void CanvasImage::drawShape ( QPainter & p )
{	
	p.drawImage( QPoint((int) x(), (int) y()), image );	
}



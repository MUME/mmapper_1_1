#ifndef _CANVAS_IMAGE_H_INCLUDED
#define _CANVAS_IMAGE_H_INCLUDED

#include <qcanvas.h>
#include <qimage.h>

class CanvasImage : public QCanvasPolygonalItem {
public:
	static int RTTI();

	CanvasImage(const QImage& a, QCanvas * canvas);
	virtual ~CanvasImage();

	virtual QPointArray areaPoints() const;
	virtual void drawShape(QPainter& p);

	virtual int rtti() const;

protected:
	QImage image;
};


#endif
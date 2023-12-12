#ifndef QQuadF_h__
#define QQuadF_h__

#include <QPointF>
#include <QMatrix3x3>
#include <QRect>
#include "EchoXCoreAPI.h"
#include "qmetatype.h"

struct ECHOXCORE_API QQuadF {
	QQuadF(QPointF topLeft = QPointF{0,0}, QPointF topRight = QPointF{ 1,0 }, QPointF bottomRight = QPointF{ 1,1 }, QPointF bottomLeft = QPointF{ 0,1 })
		: topLeft(topLeft)
		, topRight(topRight)
		, bottomRight(bottomRight)
		, bottomLeft(bottomLeft)
	{}
	QQuadF(const QRect& rect)
		: topLeft(rect.topLeft()) 
		, topRight(rect.topRight())
		, bottomRight(rect.bottomRight())
		, bottomLeft(rect.bottomLeft())
	{
	}

	QPointF topLeft;
	QPointF topRight;
	QPointF bottomRight;
	QPointF bottomLeft;

	bool contains(QPoint point);
	bool contains(QPointF point);
	void transform(const QMatrix3x3& mat);
	QRectF boundGemotry();

	QQuadF operator+(QPointF offset);
	QQuadF operator*(QPointF scale);

	static QMatrix3x3 calcTranfrom(const QQuadF& src,const QQuadF& dst);
	static QPointF transPoint(const QMatrix3x3& mat, const QPointF& src);
};

ECHOXCORE_API QDataStream& operator<<(QDataStream& out, const QQuadF& var);
ECHOXCORE_API QDataStream& operator>>(QDataStream& in, QQuadF& var);

Q_DECLARE_METATYPE(QQuadF);

#endif // QQuadF_h__

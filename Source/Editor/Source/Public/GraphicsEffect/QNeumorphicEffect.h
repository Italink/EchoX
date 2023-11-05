#ifndef QNeumorphicEffect_H
#define QNeumorphicEffect_H

#include <QGraphicsEffect>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include "EchoXEditorAPI.h"

class ECHOXEDITOR_API QNeumorphicEffect: public QGraphicsEffect
{
    Q_OBJECT

public:
	QNeumorphicEffect(qreal blurRadius = 20, qreal distance = 3, qreal strength = 0.5, qreal angle = 45, bool inset = false);
	~QNeumorphicEffect();

	qreal strength() const;
	void setStrength(const qreal& strength);

	qreal blurRadius() const;
	void setBlurRadius(const qreal& blurRadius);

	qreal distance() const;
	void setDistance(const qreal& distance);

	qreal angle() const;
	void setAngle(const qreal& angle);

	bool inset() const;
	void setInset(bool inset);

protected:
	QRectF boundingRectFor(const QRectF& rect) const override;
	void draw(QPainter* painter) override;

private:
	qreal mBlurRadius;
	qreal mDistance;
	qreal mStrength;
	qreal mAngle;
	bool bIsInset;
};

#endif // QNeumorphicEffect_H

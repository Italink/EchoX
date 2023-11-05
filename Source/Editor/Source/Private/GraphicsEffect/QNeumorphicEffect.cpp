#include "QNeumorphicEffect.h"

QNeumorphicEffect::QNeumorphicEffect(qreal blurRadius, qreal distance, qreal strength, qreal angle, bool inset)
	: mBlurRadius(blurRadius)
	, mDistance(distance)
	, mStrength(strength)
	, mAngle(angle)
	, bIsInset(inset)
{
}

QNeumorphicEffect::~QNeumorphicEffect()
{

}

qreal QNeumorphicEffect::strength() const
{
	return mStrength;
}

void QNeumorphicEffect::setStrength(const qreal& strength)
{
	mStrength = strength;
	update();
}

qreal QNeumorphicEffect::blurRadius() const
{
	return mBlurRadius;
}

void QNeumorphicEffect::setBlurRadius(const qreal& blurRadius)
{
	mBlurRadius = blurRadius;
	update();
}

qreal QNeumorphicEffect::distance() const
{
	return mDistance;
}

void QNeumorphicEffect::setDistance(const qreal& distance)
{
	mDistance = distance;
	update();
}

qreal QNeumorphicEffect::angle() const
{
	return mAngle;
}

void QNeumorphicEffect::setAngle(const qreal& angle)
{
	mAngle = angle;
	update();
}

QRectF QNeumorphicEffect::boundingRectFor(const QRectF& rect) const
{
	if (bIsInset)
		return rect.united(rect.translated(0, 0));
	return rect.united(rect.translated(0, 0).adjusted(-mBlurRadius - mDistance, -mBlurRadius - mDistance, mBlurRadius + mDistance, mBlurRadius + mDistance));
}

void QNeumorphicEffect::draw(QPainter* painter)
{
	PixmapPadMode mode = PadToEffectiveBoundingRect;
	QPoint pos;
	const QPixmap px = sourcePixmap(Qt::DeviceCoordinates, &pos, mode);
	if (px.isNull())
		return;
	QTransform restoreTransform = painter->worldTransform();
	painter->setWorldTransform(QTransform());
	if (px.isNull())
		return;

	QImage shadow1(px.size(), QImage::Format_ARGB32_Premultiplied);
	shadow1.setDevicePixelRatio(px.devicePixelRatioF());
	shadow1.fill(0);

	QImage shadow2(px.size(), QImage::Format_ARGB32_Premultiplied);
	shadow2.setDevicePixelRatio(px.devicePixelRatioF());
	shadow2.fill(0);

	QPoint offset;
	qreal radian = mAngle / 180 * M_PI;
	offset.setX(mDistance * qCos(radian));
	offset.setY(mDistance * qSin(radian));
	QT_BEGIN_NAMESPACE
		extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter * p, QImage & blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
	QT_END_NAMESPACE

		if (bIsInset) {
			QPainter shadow1Painter(&shadow1);
			shadow1Painter.drawPixmap(0, 0, px);
			shadow1Painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			shadow1Painter.fillRect(shadow1.rect(), QColor::fromRgbF(0.0, 0.0, 0.0, mStrength));
			shadow1Painter.end();

			QPainter shadow2Painter(&shadow2);
			shadow2Painter.drawPixmap(0, 0, px);
			shadow2Painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			shadow2Painter.fillRect(shadow1.rect(), QColor::fromRgbF(1.0, 1.0, 1.0, mStrength));
			shadow2Painter.end();

			QPainter shadowPainter(&shadow1);
			shadowPainter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
			shadowPainter.drawImage(offset, shadow2);
			shadowPainter.setCompositionMode(QPainter::CompositionMode_Clear);
			offset.setX(qAbs(offset.x()));
			offset.setY(qAbs(offset.y()));

			QRect rect;
			rect.setWidth(shadow1.width() / px.devicePixelRatioF() / px.devicePixelRatioF());
			rect.setHeight(shadow1.height() / px.devicePixelRatioF() / px.devicePixelRatioF());
			rect.adjust(offset.x(), offset.y(), -offset.x(), -offset.y());
			shadowPainter.fillRect(rect, Qt::transparent);
			shadowPainter.end();

			QImage blurred(px.size(), QImage::Format_ARGB32);
			blurred.setDevicePixelRatio(px.devicePixelRatioF());
			blurred.fill(0);
			QPainter blurPainter(&blurred);
			qt_blurImage(&blurPainter, shadow1, mBlurRadius, false, false);
			blurPainter.end();

			QImage image = px.toImage();
			QPainter lastPainter(&image);
			lastPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
			lastPainter.drawImage(blurred.rect(), blurred);
			lastPainter.end();

			painter->drawImage(pos, image);
		}
		else {
			QPainter shadowPainter(&shadow1);
			shadowPainter.setCompositionMode(QPainter::CompositionMode_Source);
			shadowPainter.drawPixmap(0, 0, px);
			shadowPainter.end();

			QImage blurred(shadow1.size(), QImage::Format_ARGB32_Premultiplied);
			blurred.setDevicePixelRatio(px.devicePixelRatioF());
			blurred.fill(0);

			QPainter blurPainter(&blurred);
			qt_blurImage(&blurPainter, shadow1, mBlurRadius, false, true);
			blurPainter.end();

			shadow1 = std::move(blurred);

			// blacken the image...
			shadowPainter.begin(&shadow1);
			shadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			shadowPainter.fillRect(shadow1.rect(), QColor::fromRgbF(0.0, 0.0, 0.0, mStrength));
			shadowPainter.end();
			shadow2 = shadow1;

			// blacken the image...
			shadowPainter.begin(&shadow2);
			shadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			shadowPainter.fillRect(shadow2.rect(), QColor::fromRgbF(1.0, 1.0, 1.0, mStrength));
			shadowPainter.end();

			// draw the blurred drop shadow...
			painter->drawImage(pos + offset, shadow1);
			painter->drawImage(pos - offset, shadow2);

			// Draw the actual pixmap...
			painter->drawPixmap(pos, px);
		}
	painter->setWorldTransform(restoreTransform);
}

bool QNeumorphicEffect::inset() const
{
	return bIsInset;
}

void QNeumorphicEffect::setInset(bool inset)
{
	bIsInset = inset;
	update();
}
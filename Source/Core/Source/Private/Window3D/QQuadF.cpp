#include "QQuadF.h"

void QQuadF::transform(const QMatrix3x3& mat)
{
	topLeft = transPoint(mat, topLeft);
	topRight = transPoint(mat, topRight);
	bottomRight = transPoint(mat, bottomRight);
	bottomLeft = transPoint(mat, bottomLeft);
}

QRectF QQuadF::boundGemotry()
{
	float minX, minY, maxX, maxY;
	minX = std::min({ topLeft.x(),topRight.x(),bottomRight.x(),bottomLeft.x() });
	minY = std::min({ topLeft.y(),topRight.y(),bottomRight.y(),bottomLeft.y() });
	maxX = std::max({ topLeft.x(),topRight.x(),bottomRight.x(),bottomLeft.x() });
	maxY = std::max({ topLeft.y(),topRight.y(),bottomRight.y(),bottomLeft.y() });
	return QRectF(minX, minY, maxX - minX, maxY - minY);
}

QQuadF QQuadF::operator+(QPointF offset)
{
	QQuadF newQuad = *this;
	newQuad.topLeft += offset;
	newQuad.topRight += offset;
	newQuad.bottomRight += offset;
	newQuad.bottomLeft += offset;
	return newQuad;
}

QQuadF QQuadF::operator*(QPointF scale)
{
	QQuadF newQuad = *this;
	newQuad.topLeft = { newQuad.topLeft.x() * scale.x(),newQuad.topLeft.y() * scale.y() };
	newQuad.topRight = { newQuad.topRight.x() * scale.x(),newQuad.topRight.y() * scale.y() };
	newQuad.bottomRight = { newQuad.bottomRight.x() * scale.x(),newQuad.bottomRight.y() * scale.y() };
	newQuad.bottomLeft = { newQuad.bottomLeft.x() * scale.x(),newQuad.bottomLeft.y() * scale.y() };
	return newQuad;
}

QMatrix3x3 QQuadF::calcTranfrom(const QQuadF& src, const QQuadF& dst)
{
	QPointF srcGeomtry3D[4] = {
		 src.bottomLeft ,src.bottomRight ,src.topRight ,src.topLeft
	};
	QPointF dstGeomtry3D[4] = {
		dst.bottomLeft ,dst.bottomRight ,dst.topRight ,dst.topLeft
	};
	QMatrix3x3 mat;
	float x0 = srcGeomtry3D[0].x(), x1 = srcGeomtry3D[1].x(), x2 = srcGeomtry3D[3].x(), x3 = srcGeomtry3D[2].x();
	float y0 = srcGeomtry3D[0].y(), y1 = srcGeomtry3D[1].y(), y2 = srcGeomtry3D[3].y(), y3 = srcGeomtry3D[2].y();
	float u0 = dstGeomtry3D[0].x(), u1 = dstGeomtry3D[1].x(), u2 = dstGeomtry3D[3].x(), u3 = dstGeomtry3D[2].x();
	float v0 = dstGeomtry3D[0].y(), v1 = dstGeomtry3D[1].y(), v2 = dstGeomtry3D[3].y(), v3 = dstGeomtry3D[2].y();

	float A[8][9] = {
		   { x0, y0, 1, 0, 0, 0, -x0 * u0, -y0 * u0, u0 },
		   { x1, y1, 1, 0, 0, 0, -x1 * u1, -y1 * u1, u1 },
		   { x2, y2, 1, 0, 0, 0, -x2 * u2, -y2 * u2, u2 },
		   { x3, y3, 1, 0, 0, 0, -x3 * u3, -y3 * u3, u3 },
		   { 0, 0, 0, x0, y0, 1, -x0 * v0, -y0 * v0, v0 },
		   { 0, 0, 0, x1, y1, 1, -x1 * v1, -y1 * v1, v1 },
		   { 0, 0, 0, x2, y2, 1, -x2 * v2, -y2 * v2, v2 },
		   { 0, 0, 0, x3, y3, 1, -x3 * v3, -y3 * v3, v3 },
	};

	for (int row = 0, col = 0; col < 8 && row < 8; col++, row++) {      //高斯消元
		int max_r = row;
		for (int i = row + 1; i < 8; i++) {
			if ((1e-12) < qAbs(A[i][col]) - qAbs(A[max_r][col])) {
				max_r = i;
			}
		}
		if (max_r != row)
			for (int j = 0; j < 9; j++)
				qSwap(A[row][j], A[max_r][j]);
		for (int i = row + 1; i < 8; i++) {
			if (fabs(A[i][col]) < (1e-12))
				continue;
			float tmp = -A[i][col] / A[row][col];
			for (int j = col; j < 9; j++) {
				A[i][j] += tmp * A[row][j];
			}
		}

	}
	for (int i = 7; i >= 0; i--) { //计算唯一解。
		float tmp = 0;
		for (int j = i + 1; j < 8; j++) {
			tmp += A[i][j] * mat(j / 3, j % 3);
		}
		mat(i / 3, i % 3) = (A[i][8] - tmp) / A[i][i];
	}
	mat(2, 2) = 1;
	return mat;
}

QPointF QQuadF::transPoint(const QMatrix3x3& mat, const QPointF& src)
{
	QPointF ret;
	float z = src.x() * mat(2, 0) + src.y() * mat(2, 1) + mat(2, 2);
	ret.setX((src.x() * mat(0, 0) + src.y() * mat(0, 1) + mat(0, 2)) / z);
	ret.setY((src.x() * mat(1, 0) + src.y() * mat(1, 1) + mat(1, 2)) / z);
	return ret;
}

QDataStream& operator<<(QDataStream& out, const QQuadF& var)
{
	return out << var.topLeft
		<< var.topRight
		<< var.bottomRight
		<< var.bottomLeft;
}

QDataStream& operator>>(QDataStream& in, QQuadF& var)
{
	return in >> var.topLeft
		>> var.topRight
		>> var.bottomRight
		>> var.bottomLeft;
}

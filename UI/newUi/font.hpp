#pragma once

#include <qfont.h>
#include <qguiapplication.h>
#include <qscreen.h>
#include <qdebug.h>
#include <qpainterpath.h>

enum FontWeight {
	Blod = 700,
	Regular = 380,
	Light = 290
};

static qreal getScale()
{
	static int m_scale = 0;
	if (0 == m_scale) {
		QScreen *primaryScreen = QGuiApplication::primaryScreen();
		uint32_t cx = primaryScreen->size().width();
		m_scale = (cx >= 3840 ? 2 : 1);
	}

	return m_scale;
}

static QFont getFont(int pixSize = 12, QFont::Weight weight = QFont::Normal)
{
	QFont font;
	font.setFamily("Microsoft YaHei UI");
	font.setPixelSize(pixSize * getScale());
	font.setWeight(weight);
	return font;
}

static QString getFontStyle(int pixSize = 12, FontWeight weight = FontWeight::Regular)
{
	return QString("font: %1 %2px 'Microsoft YaHei UI';").arg(weight).arg(pixSize * getScale());
}

static QPainterPath getRoundRectPath(const QRectF &rect, qreal topLeftRadius,
				     qreal topRightRadius,
				     qreal bottomLeftRadius,
				     qreal bottomRightRadius)
{
	qreal topLeftSubValue = topLeftRadius > 5 ? topLeftRadius * 1.5 / 12.0
						  : 0;
	qreal topRightSubValue =
		topRightRadius > 5 ? topRightRadius * 1.5 / 12.0 : 0;
	qreal bottomRightSubValue =
		bottomRightRadius > 5 ? bottomRightRadius * 1.5 / 12.0 : 0;
	qreal bottomLeftSubValue =
		bottomLeftRadius > 5 ? bottomLeftRadius * 1.5 / 12.0 : 0;
	qreal subValue = topLeftSubValue;

	if (subValue != topRightSubValue || subValue != bottomRightSubValue ||
	    subValue != bottomLeftSubValue)
		subValue = 0;

	QPainterPath path;
	QPointF topLeftP1 =
		QPointF(rect.x() + subValue, rect.y() + topLeftRadius);
	QPointF topLeftP2 =
		QPointF(rect.x() + topLeftRadius, rect.y() + subValue);
	QPointF topRightP1 = QPointF(rect.x() + rect.width() - topRightRadius,
				     rect.y() + subValue);
	QPointF topRightP2 = QPointF(rect.x() + rect.width() - subValue,
				     rect.y() + topRightRadius);
	QPointF bottomRightP1 =
		QPointF(rect.x() + rect.width() - subValue,
			rect.y() + rect.height() - bottomLeftRadius);
	QPointF bottomRightP2 =
		QPointF(rect.x() + rect.width() - bottomLeftRadius,
			rect.y() + rect.height() - subValue);
	QPointF bottomLeftP1 = QPointF(rect.x() + bottomRightRadius,
				       rect.y() + rect.height() - subValue);
	QPointF bottomLeftP2 =
		QPointF(rect.x() + subValue,
			rect.y() + rect.height() - bottomRightRadius);
	path.moveTo(topLeftP1);
	path.cubicTo(topLeftP1, rect.topLeft(), topLeftP2);
	path.lineTo(topRightP1);
	path.cubicTo(topRightP1, rect.topRight(), topRightP2);
	path.lineTo(bottomRightP1);
	path.cubicTo(bottomRightP1, rect.bottomRight(), bottomRightP2);
	path.lineTo(bottomLeftP1);
	path.cubicTo(bottomLeftP1, rect.bottomLeft(), bottomLeftP2);
	path.lineTo(topLeftP1);
	return path;
}

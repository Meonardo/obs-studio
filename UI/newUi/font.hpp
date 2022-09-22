#pragma once

#include <qfont.h>
#include <qguiapplication.h>
#include <qscreen.h>
#include <qdebug.h>

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

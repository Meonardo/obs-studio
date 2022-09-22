#include "window-ai-checkbutton.hpp"
#include <qguiapplication.h>
#include <qscreen.h>
#include <qpainter.h>

OBSAiCheckButton::OBSAiCheckButton(QWidget *parent)
	: QWidget(parent)
{
}

OBSAiCheckButton::~OBSAiCheckButton() {}

void OBSAiCheckButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(this->rect(), QColor(0, 0, 0, 0));
	painter.setBrush(QColor(46, 105, 240));
	painter.drawRoundRect(this->rect(), this->width() / 2, this->height() / 2);
}

void OBSAiCheckButton::initUi()
{
	
}


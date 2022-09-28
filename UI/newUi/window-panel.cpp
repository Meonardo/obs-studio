#include "window-panel-main.hpp"
#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qguiapplication.h>
#include <qscreen.h>
#include <qevent.h>
#include <qlistwidget.h>
#include <qdebug.h>
#include "newUi/font.hpp"
#include "font.hpp"

OBSPanel::OBSPanel(const QString &title, QWidget *parent)
	: m_title(title), QWidget(parent)
{
	this->initUi();
}

OBSPanel::~OBSPanel() {}

void OBSPanel::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(this->rect(), QColor(68, 68, 68, 0));
	return QWidget::paintEvent(event);
}

bool OBSPanel::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		if (obj != this->pBtn_add)
			return QWidget::eventFilter(obj, event);
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			emit addClicked();
		}
	}
	return QWidget::eventFilter(obj, event);
}

void OBSPanel::initUi()
{
	QFrame *frame_top = new QFrame(this);
	frame_top->setStyleSheet("QFrame{background-color: '#444444';}");
	frame_top->setFixedHeight(40 * getScale());

	QLabel *label_title = new QLabel(frame_top);
	label_title->setText(m_title);
	label_title->setStyleSheet(QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	this->pBtn_add = new QLabel(frame_top);
	this->pBtn_add->setFixedSize(24 * getScale(), 24 * getScale());
	this->pBtn_add->setStyleSheet("QLabel{ border-image: url(':/res/images/newUi/tianjia@2x.png');}");
	this->pBtn_add->installEventFilter(this);

	auto layout_top = new QHBoxLayout(frame_top);
	layout_top->setContentsMargins(16 * getScale(), 0, 16 * getScale(), 0);
	layout_top->addWidget(label_title, 1);
	layout_top->addWidget(pBtn_add, 0, Qt::AlignVCenter);

	QListWidget *listwidget = new QListWidget(this);
	listwidget->setStyleSheet("QListWidget{background-color: '#444444'; border-radius: 0;}");

	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1 * getScale());
	layout->addWidget(frame_top, 0);
	layout->addWidget(listwidget, 1);
}


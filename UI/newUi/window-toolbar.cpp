#include "window-toolbar.hpp"
#include <qcheckbox.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qdebug.h>
#include <qevent.h>
#include "font.hpp"
#include "obs-app.hpp"

OBSToolbar::OBSToolbar(QWidget *parent) : QWidget(parent)
{
	this->initUi();
}

OBSToolbar::~OBSToolbar() {}

void OBSToolbar::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(this->rect(), QColor(68, 68, 68));
	return QWidget::paintEvent(event);
}

bool OBSToolbar::eventFilter(QObject *obj, QEvent *event)
{
	if ((obj == pBtn_settings_stream ||
		obj == label_settings_stream) &&
	    event->type() == QEvent::Enter) {
		pBtn_settings_stream->setStyleSheet(
			QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
				"border-image: url(':/res/images/newUi/settings2@2x.png'); "
				"min-width: %1; min-height: %1;}")
				.arg(16 * getScale()));
		label_settings_stream->setStyleSheet(
			QString("QLabel{ color: white; padding-left: %2px; %1}")
				.arg(getFontStyle(12))
				.arg(3 * getScale()));
	} else if ((obj == pBtn_settings_stream ||
		    obj == label_settings_stream) &&
		   event->type() == QEvent::Leave) {
		//QMouseEvent *mouseEvent =
		//	static_cast<QMouseEvent *>(event);
		//if (pBtn_settings_stream->rect().contains(mouseEvent->pos()) ||
		//    label_settings_stream->rect().contains(mouseEvent->pos()))
		//	return QWidget::eventFilter(obj, event);
		
		pBtn_settings_stream->setStyleSheet(
			QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
				"border-image: url(':/res/images/newUi/settings@2x.png'); "
				"min-width: %1; min-height: %1;}")
				.arg(16 * getScale()));
		label_settings_stream->setStyleSheet(
			QString("QLabel{ color: rgb(170,170,170); padding-left: %2px; %1}")
				.arg(getFontStyle(12))
				.arg(3 * getScale()));
	} else if ((obj == pBtn_settings_stream ||
		    obj == label_settings_stream) &&
		   event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseEvent =
			static_cast<QMouseEvent *>(event);
		if (mouseEvent->button() == Qt::LeftButton)
			emit showStreamPanel();
	}
	return QWidget::eventFilter(obj, event);
}

void OBSToolbar::initUi()
{
	QLabel *label = new QLabel(this);
	label->setText(QTStr("NewUi.Guide"));
	label->setStyleSheet(
		QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));
	label->hide();

	QFrame *switch_mode = new QFrame(this);
	switch_mode->setFixedSize(104 * getScale(), 28 * getScale());
	switch_mode->setStyleSheet(
		"QFrame{ background-color: rgb(102,102,102);}");
	switch_mode->hide();

	QLabel *label_line = new QLabel(this);
	label_line->setFixedSize(1 * getScale(), 28 * getScale());
	label_line->setStyleSheet("QLabel{background-color: rgb(34,34,34);}");
	label_line->hide();

	QLabel *label_1 = new QLabel(this);
	label_1->setText(QTStr("NewUi.AiBlackboard"));
	label_1->setStyleSheet(
		QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));
	label_1->hide();

	QCheckBox *checkbox_ai = new QCheckBox(this);
	checkbox_ai->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_ai->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_ai->width())
			.arg(checkbox_ai->height()));
	checkbox_ai->hide();

	QPushButton *pBtn_settings_ai = new QPushButton(this);
	pBtn_settings_ai->setFixedSize(16 * getScale(), 16 * getScale());
	pBtn_settings_ai->setStyleSheet(
		QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
			"border-image: url(':/res/images/newUi/settings@2x.png'); "
			"min-width: %1; min-height: %1;} "
			"QPushButton::hover{border-image: url(':/res/images/newUi/settings2@2x.png');}")
			.arg(16 * getScale()));
	pBtn_settings_ai->setEnabled(false);
	pBtn_settings_ai->hide();

	QLabel *label_settings_ai = new QLabel(this);
	label_settings_ai->setText(QTStr("NewUi.Settings"));
	label_settings_ai->setStyleSheet(
		QString("QLabel{ color: rgb(170,170,170); %1}")
			.arg(getFontStyle(12)));
	label_settings_ai->hide();

	QLabel *label_line_2 = new QLabel(this);
	label_line_2->setFixedSize(1 * getScale(), 28 * getScale());
	label_line_2->setStyleSheet("QLabel{background-color: rgb(34,34,34);}");
	label_line_2->hide();

	QLabel *label_2 = new QLabel(this);
	label_2->setText(QTStr("NewUi.Streaming"));
	label_2->setStyleSheet(
		QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	pBtn_settings_stream = new QPushButton(this);
	pBtn_settings_stream->setFixedSize(16 * getScale(), 16 * getScale());
	pBtn_settings_stream->setStyleSheet(
		QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
			"border-image: url(':/res/images/newUi/settings@2x.png'); "
			"min-width: %1; min-height: %1;}")
			.arg(16 * getScale()));
	pBtn_settings_stream->installEventFilter(this);

	QCheckBox *checkbox_stream = new QCheckBox(this);
	checkbox_stream->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_stream->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_stream->width())
			.arg(checkbox_stream->height()));
	connect(checkbox_stream, &QCheckBox::stateChanged, this,
		[=](int state) {
			//if (0 == state)
			//	pBtn_settings_stream->setEnabled(false);
			//else
			//	pBtn_settings_stream->setEnabled(true);
		});

	label_settings_stream = new QLabel(this);
	label_settings_stream->setText(QTStr("NewUi.Settings"));
	label_settings_stream->installEventFilter(this);
	label_settings_stream->setStyleSheet(
		QString("QLabel{ color: rgb(170,170,170); padding-left: %2px; %1}")
			.arg(getFontStyle(12)).arg(3 * getScale()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	layout->addWidget(label, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(switch_mode, 0, Qt::AlignVCenter);

	layout->addSpacerItem(
		new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_line, 0, Qt::AlignVCenter);

	layout->addSpacerItem(
		new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_1, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(checkbox_ai, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(16 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(pBtn_settings_ai, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(6 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_settings_ai, 0, Qt::AlignVCenter);

	layout->addSpacerItem(
		new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_line_2, 0, Qt::AlignVCenter);

	layout->addSpacerItem(
		new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_2, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(checkbox_stream, 0, Qt::AlignVCenter);
	layout->addSpacerItem(
		new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(pBtn_settings_stream, 0, Qt::AlignVCenter);
	//layout->addSpacerItem(
	//	new QSpacerItem(6 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_settings_stream, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
}

#include "window-toolbar.hpp"
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qdebug.h>
#include "font.hpp"

OBSToolbar::OBSToolbar(QWidget *parent)
	: QWidget(parent)
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

void OBSToolbar::initUi()
{
	QLabel *label = new QLabel(this);
	label->setText(tr("导播"));
	label->setStyleSheet(QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	QFrame *switch_mode = new QFrame(this);
	switch_mode->setFixedSize(104 * getScale(), 28 * getScale());
	switch_mode->setStyleSheet("QFrame{ background-color: rgb(102,102,102);}");

	QLabel *label_line = new QLabel(this);
	label_line->setFixedSize(1 * getScale(), 28 * getScale());
	label_line->setStyleSheet("QLabel{background-color: rgb(34,34,34);}");

	QLabel *label_1 = new QLabel(this);
	label_1->setText(tr("AI板书"));
	label_1->setStyleSheet(QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	QCheckBox *checkbox_ai = new QCheckBox(this);
	checkbox_ai->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_ai->setStyleSheet(QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_ai->width()).arg(checkbox_ai->height()));

	QPushButton *pBtn_settings_ai = new QPushButton(this);
	pBtn_settings_ai->setFixedSize(16 * getScale(), 16 * getScale());
	pBtn_settings_ai->setStyleSheet(QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
					"border-image: url(':/res/images/newUi/settings@2x.png'); "
					"min-width: %1; min-height: %1;}").arg(16 * getScale()));

	QLabel *label_settings_ai = new QLabel(this);
	label_settings_ai->setText(tr("设置"));
	label_settings_ai->setStyleSheet(QString("QLabel{ color: rgb(170,170,170); %1}").arg(getFontStyle(12)));

	QLabel *label_line_2 = new QLabel(this);
	label_line_2->setFixedSize(1 * getScale(), 28 * getScale());
	label_line_2->setStyleSheet("QLabel{background-color: rgb(34,34,34);}");

	QLabel *label_2 = new QLabel(this);
	label_2->setText(tr("推流"));
	label_2->setStyleSheet(QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	QCheckBox *checkbox_stream = new QCheckBox(this);
	checkbox_stream->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_stream->setStyleSheet(QString("QCheckBox { background-color: transparent; border: none;} "
		"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
		"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
		"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}").arg(checkbox_stream->width()).arg(checkbox_stream->height()));

	QPushButton *pBtn_settings_stream = new QPushButton(this);
	pBtn_settings_stream->setFixedSize(16 * getScale(), 16 * getScale());
	pBtn_settings_stream->setStyleSheet(QString("QPushButton{ padding: 0px; border: none; background-color: transparent;"
					"border-image: url(':/res/images/newUi/settings@2x.png'); "
					"min-width: %1; min-height: %1;}").arg(16 * getScale()));
	connect(pBtn_settings_stream, & QPushButton::clicked, this, [=](){ emit showStreamPanel(); });

	QLabel *label_settings_stream = new QLabel(this);
	label_settings_stream->setText(tr("设置"));
	label_settings_stream->setStyleSheet(QString("QLabel{ color: rgb(170,170,170); %1}").arg(getFontStyle(12)));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	layout->addWidget(label, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(switch_mode, 0, Qt::AlignVCenter);

	layout->addSpacerItem(new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_line, 0, Qt::AlignVCenter);

	layout->addSpacerItem(new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_1, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(checkbox_ai, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(16 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(pBtn_settings_ai, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(6 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_settings_ai, 0, Qt::AlignVCenter);

	layout->addSpacerItem(new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_line_2, 0, Qt::AlignVCenter);

	layout->addSpacerItem(new QSpacerItem(32 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_2, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(checkbox_stream, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(pBtn_settings_stream, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(10 * getScale(), 1, QSizePolicy::Fixed));
	layout->addWidget(label_settings_stream, 0, Qt::AlignVCenter);
	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
}

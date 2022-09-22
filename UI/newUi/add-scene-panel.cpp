#include "add-scene-panel.hpp"
#include <qbuttongroup.h>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
	this->setMaxVisibleItems(6);
	this->setStyleSheet(
		QString("QComboBox{background-color: rgba(240,240,240); border-radius: 4; color: rgb(68,68,68); %1}"
			"QComboBox::drop-down{width:57 px; border: none;}"
			"QComboBox::down-arrow {width: %2px; height: %3px; image: url(':/res/images/newUi/arrow@2x.png');}"
			"QComboBox::down-arrow:on {image: url(':/res/images/newUi/arrow_on@2x.png');}"
			"QComboBox QAbstractItemView { outline: 1px solid rgba(238,238,238); background-color: white; border: 1px solid rgba(238,238,238); border-radius: 8px;}"
			"QComboBox QAbstractItemView::item {background-color: white; height: %4px; %1 color: rgb(68,68,68);}"
			"QComboBox QAbstractItemView::item:hover {color: #FFFFFF;background-color: #554433;}")
			.arg(getFontStyle(18))
			.arg(15 * getScale())
			.arg(10 * getScale())
			.arg(60 * getScale()));
	ItemDelegate *delegate = new ItemDelegate(this);
	this->setItemDelegate(delegate);
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
			 const QModelIndex &index) const
{
	QSize size = this->sizeHint(option, index);
	//if (!option.state.testFlag(QStyle::State_Selected) &&
	//	    option.state.testFlag(QStyle::State_MouseOver)) {

		painter->fillRect(0, index.row() * size.height(), size.width(),
				  size.height(), QBrush(QColor(239, 243, 254)));
	//}

	if (option.state.testFlag(QStyle::State_Selected)) {
		painter->setPen(QPen(QColor(46, 105, 240)));
	} else {
		painter->setPen(QPen(QColor(68, 68, 68)));
	}

	painter->setFont(getFont(16));

	QRect boundRect;
	painter->drawText(QRect(21 * getScale(), index.row() * size.height(),
				size.width(), size.height()),
			  Qt::AlignLeft | Qt::AlignVCenter,
			  index.data().toString(), &boundRect);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option,
			     const QModelIndex &index) const
{
	return QSize(410 * getScale(), 55 * getScale());
}

void SceneSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QString::fromLocal8Bit("板书画面"), this);
	label->setFixedWidth(72 * getScale());
	combobox_scenes = new ComboBox(this);
	combobox_scenes->setFixedSize(410 * getScale(), 51 * getScale());

	label->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
				     .arg(getFontStyle(18)));

	combobox_scenes->addItem(QString::fromLocal8Bit("老师特写"));
	combobox_scenes->addItem(QString::fromLocal8Bit("学生镜头"));
	combobox_scenes->addItem(QString::fromLocal8Bit("板书"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));
	combobox_scenes->addItem(QString::fromLocal8Bit("桌面"));

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout->addWidget(label, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(combobox_scenes, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_1 = new QLabel(QString::fromLocal8Bit("帧率"), this);
	QLabel *label_2 = new QLabel(QString::fromLocal8Bit("编码"), this);
	label_1->setStyleSheet(label->styleSheet());
	label_2->setStyleSheet(label->styleSheet());
	label_1->setFixedWidth(label->width());
	label_2->setFixedWidth(35 * getScale());
	label_2->setAlignment(Qt::AlignRight);
	combobox_fps = new ComboBox(this);
	combobox_fps->setFixedSize(139 * getScale(), 51 * getScale());
	combobox_encode = new ComboBox(this);
	combobox_encode->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_1 = new QHBoxLayout;
	hlayout_1->setContentsMargins(0, 0, 0, 0);
	hlayout_1->setSpacing(0);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_1->addWidget(label_1, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(combobox_fps, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(66 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(label_2, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(31 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(combobox_encode, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_3 = new QLabel(QString::fromLocal8Bit("分辨率"), this);
	label_3->setStyleSheet(label->styleSheet());
	label_3->setFixedWidth(label->width());
	combobox_resolution = new ComboBox(this);
	combobox_resolution->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_2 = new QHBoxLayout;
	hlayout_2->setContentsMargins(0, 0, 0, 0);
	hlayout_2->setSpacing(0);
	hlayout_2->addSpacerItem(
		new QSpacerItem(50 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(label_3, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(combobox_resolution, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addLayout(hlayout);
	vlayout->addLayout(hlayout_1);
	vlayout->addLayout(hlayout_2);
}

void IpCameraSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QString::fromLocal8Bit("摄像头名称"), this);
	label->setFixedWidth(90 * getScale());
	label->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
				     .arg(getFontStyle(18)));
	combobox_cameraName = new ComboBox(this);
	combobox_cameraName->setFixedSize(410 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout->addWidget(label, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(combobox_cameraName, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_1 = new QLabel(QString::fromLocal8Bit("RTSP"), this);
	label_1->setFixedWidth(label->width());
	label_1->setStyleSheet(label->styleSheet());
	lineedit_rtsp = new QLineEdit(this);
	lineedit_rtsp->setFixedSize(410 * getScale(), 51 * getScale());
	lineedit_rtsp->setStyleSheet(QString("QLineEdit{ background-color:rgb(240,240,240); border-radius: %1px;}").arg(4 * getScale()));
	lineedit_rtsp->setPlaceholderText(QString::fromLocal8Bit("输入RTSP"));
	lineedit_rtsp->setFont(getFont(18));
	QPalette palette = lineedit_rtsp->palette();
	palette.setColor(QPalette::Normal, QPalette::PlaceholderText, QColor(170, 170, 170));
	lineedit_rtsp->setPalette(palette);

	QHBoxLayout *hlayout_1 = new QHBoxLayout;
	hlayout_1->setContentsMargins(0, 0, 0, 0);
	hlayout_1->setSpacing(0);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_1->addWidget(label_1, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(lineedit_rtsp, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_2 = new QLabel(QString::fromLocal8Bit("码率"), this);
	QLabel *label_3 = new QLabel(QString::fromLocal8Bit("帧率"), this);
	label_2->setFixedWidth(label->width());
	label_3->setFixedWidth(36 * getScale());
	label_2->setStyleSheet(label->styleSheet());
	label_3->setStyleSheet(label->styleSheet());
	label_3->setAlignment(Qt::AlignRight);
	combobox_rate = new ComboBox(this);
	combobox_rate->setFixedSize(139 * getScale(), 51 * getScale());
	combobox_fps = new ComboBox(this);
	combobox_fps->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_2 = new QHBoxLayout;
	hlayout_2->setContentsMargins(0, 0, 0, 0);
	hlayout_2->setSpacing(0);
	hlayout_2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_2->addWidget(label_2, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(combobox_rate, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(66 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(label_3, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(combobox_fps, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_4 = new QLabel(QString::fromLocal8Bit("编码"), this);
	QLabel *label_5 = new QLabel(QString::fromLocal8Bit("分辨率"), this);
	label_4->setFixedWidth(label->width());
	label_5->setFixedWidth(64 * getScale());
	label_4->setStyleSheet(label->styleSheet());
	label_5->setStyleSheet(label->styleSheet());
	label_5->setAlignment(Qt::AlignRight);
	combobox_encode = new ComboBox(this);
	combobox_encode->setFixedSize(139 * getScale(), 51 * getScale());
	combobox_resolution = new ComboBox(this);
	combobox_resolution->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_3 = new QHBoxLayout;
	hlayout_3->setContentsMargins(0, 0, 0, 0);
	hlayout_3->setSpacing(0);
	hlayout_3->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_3->addWidget(label_4, 0, Qt::AlignVCenter);
	hlayout_3->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_3->addWidget(combobox_encode, 0, Qt::AlignVCenter);
	hlayout_3->addSpacerItem(
		new QSpacerItem(38 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_3->addWidget(label_5, 0, Qt::AlignVCenter);
	hlayout_3->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_3->addWidget(combobox_resolution, 0, Qt::AlignVCenter);
	hlayout_3->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addLayout(hlayout);
	vlayout->addLayout(hlayout_1);
	vlayout->addLayout(hlayout_2);
	vlayout->addLayout(hlayout_3);
}

void USBCameraSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QString::fromLocal8Bit("摄像头名称"), this);
	label->setFixedWidth(90 * getScale());
	label->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
				     .arg(getFontStyle(18)));
	combobox_cameraName = new ComboBox(this);
	combobox_cameraName->setFixedSize(410 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout->addWidget(label, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(combobox_cameraName, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_1 = new QLabel(QString::fromLocal8Bit("码率"), this);
	QLabel *label_2 = new QLabel(QString::fromLocal8Bit("帧率"), this);
	label_1->setFixedWidth(label->width());
	label_2->setFixedWidth(36 * getScale());
	label_1->setStyleSheet(label->styleSheet());
	label_2->setStyleSheet(label->styleSheet());
	label_2->setAlignment(Qt::AlignRight);
	combobox_rate = new ComboBox(this);
	combobox_rate->setFixedSize(139 * getScale(), 51 * getScale());
	combobox_fps = new ComboBox(this);
	combobox_fps->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_1 = new QHBoxLayout;
	hlayout_1->setContentsMargins(0, 0, 0, 0);
	hlayout_1->setSpacing(0);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_1->addWidget(label_1, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(combobox_rate, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(66 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(label_2, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(combobox_fps, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_3 = new QLabel(QString::fromLocal8Bit("编码"), this);
	QLabel *label_4 = new QLabel(QString::fromLocal8Bit("分辨率"), this);
	label_3->setFixedWidth(label->width());
	label_4->setFixedWidth(64 * getScale());
	label_3->setStyleSheet(label->styleSheet());
	label_4->setStyleSheet(label->styleSheet());
	label_4->setAlignment(Qt::AlignRight);
	combobox_encode = new ComboBox(this);
	combobox_encode->setFixedSize(139 * getScale(), 51 * getScale());
	combobox_resolution = new ComboBox(this);
	combobox_resolution->setFixedSize(139 * getScale(), 51 * getScale());

	QHBoxLayout *hlayout_2 = new QHBoxLayout;
	hlayout_2->setContentsMargins(0, 0, 0, 0);
	hlayout_2->setSpacing(0);
	hlayout_2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_2->addWidget(label_3, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(combobox_encode, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(38 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(label_4, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_2->addWidget(combobox_resolution, 0, Qt::AlignVCenter);
	hlayout_2->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addLayout(hlayout);
	vlayout->addLayout(hlayout_1);
	vlayout->addLayout(hlayout_2);
}

AddScenesPanel::AddScenesPanel(QWidget *parent) : QWidget(parent)
{
	this->initUi();
	this->installEventFilter(this);
}

AddScenesPanel::~AddScenesPanel() {}

void AddScenesPanel::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(this->rect(), QColor(0, 0, 0, 0));
	painter.setBrush(QColor(255, 255, 255));
	painter.drawRoundedRect(this->rect(), 12 * getScale(), 12 * getScale());
	return QWidget::paintEvent(event);
}

bool AddScenesPanel::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == pBtn_close && event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			this->close();
			this->deleteLater();
		}
	} else if (obj == this && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			mousePressed = true;
			cPos = mouseevent->pos();
		}
	} else if (obj == this && event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			mousePressed = false;
		}
	} else if (obj == this && event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		QPoint gPos = mouseevent->pos();
		this->move(this->x() + gPos.x() - cPos.x(),
			   this->y() + gPos.y() - cPos.y());
	} else if (obj != this && event->type() == QEvent::MouseButtonPress) {
		event->accept();
		return false;
	}
	return QWidget::eventFilter(obj, event);
}

void AddScenesPanel::initUi()
{
	auto frame_top = new QFrame(this);
	frame_top->setFixedHeight(70 * getScale());

	QLabel *label_title = new QLabel(frame_top);
	label_title->setText(QString::fromLocal8Bit("添加主画面"));
	label_title->setStyleSheet(
		QString("QLabel{color:rgba(34,34,34); %1}")
			.arg(getFontStyle(22, FontWeight::Blod)));

	pBtn_close = new QLabel(frame_top);
	pBtn_close->setFixedSize(36 * getScale(), 36 * getScale());
	pBtn_close->setStyleSheet(
		"QLabel{border-image: url(':/res/images/newUi/close@2x.png');}");
	pBtn_close->installEventFilter(this);

	QHBoxLayout *layout_top = new QHBoxLayout(frame_top);
	layout_top->setContentsMargins(30 * getScale(), 0, 24 * getScale(), 0);
	layout_top->addWidget(label_title, 1, Qt::AlignVCenter);
	layout_top->addWidget(pBtn_close, 0, Qt::AlignVCenter);

	auto frame_tab = new QFrame(this);
	frame_tab->setFixedHeight(80 * getScale());

	tabBtn_1 = new QPushButton(QString::fromLocal8Bit("板书"), frame_tab);
	tabBtn_1->setCheckable(true);
	tabBtn_1->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_1->setStyleSheet(
		QString("QPushButton{ background-color: rgb(239,243,254); color: rgb(102,102,102); border-radius: 0; "
			"border-bottom-left-radius:%1px; border-top-left-radius: %1px; %2}"
			"QPushButton::checked{ background-color: rgb(46,105,240); color: white;"
			"border: %3 px solid rgba(57,103,203,0.2);}")
			.arg(2 * getScale())
			.arg(getFontStyle(16))
			.arg(1 * getScale()));
	connect(tabBtn_1, &QPushButton::clicked, this, [=]() {
		this->stackedWidget->setCurrentWidget(sceneSettingsWidget);
		this->setFixedSize(612 * getScale(), 496 * getScale());
	});

	tabBtn_2 = new QPushButton(QString(tr("IP Camera")), frame_tab);
	tabBtn_2->setCheckable(true);
	tabBtn_2->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_2->setStyleSheet(
		QString("QPushButton{ background-color: rgb(239,243,254); color: rgb(102,102,102); border-radius: 0; %1}"
			"QPushButton::checked{ background-color: rgb(46,105,240); color: white;"
			"border: %2 px solid rgba(57,103,203,0.2); border-radius: 0;}")
			.arg(getFontStyle(16))
			.arg(1 * getScale()));
	connect(tabBtn_2, &QPushButton::clicked, this, [=]() {
		this->stackedWidget->setCurrentWidget(ipCameraSettingsWidget);
		this->setFixedSize(612 * getScale(), 537 * getScale());
	});

	tabBtn_3 = new QPushButton(QString(tr("USB Camera")), frame_tab);
	tabBtn_3->setCheckable(true);
	tabBtn_3->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_3->setStyleSheet(
		QString("QPushButton{ background-color: rgb(239,243,254); color: rgb(102,102,102); border-radius: 0;"
			"border-bottom-right-radius:%1px; border-top-right-radius: %1px; %2}"
			"QPushButton::checked{ background-color: rgb(46,105,240); color: white;"
			"border: %3 px solid rgba(57,103,203,0.2);}")
			.arg(2 * getScale())
			.arg(getFontStyle(16))
			.arg(1 * getScale()));
	connect(tabBtn_3, &QPushButton::clicked, this, [=]() {
		this->stackedWidget->setCurrentWidget(usbCameraSettingsWidget);
		this->setFixedSize(612 * getScale(), 496 * getScale());
	});

	QHBoxLayout *layout_tab = new QHBoxLayout(frame_tab);
	layout_tab->setSpacing(0);
	layout_tab->addSpacerItem(
		new QSpacerItem(1, 1, QSizePolicy::Expanding));
	layout_tab->addWidget(tabBtn_1, 0, Qt::AlignVCenter);
	layout_tab->addWidget(tabBtn_2, 0, Qt::AlignVCenter);
	layout_tab->addWidget(tabBtn_3, 0, Qt::AlignVCenter);
	layout_tab->addSpacerItem(
		new QSpacerItem(1, 1, QSizePolicy::Expanding));

	stackedWidget = new QStackedWidget(this);
	stackedWidget->setFixedWidth(612 * getScale());

	auto frame_bottom = new QFrame(this);
	frame_bottom->setFixedHeight(121 * getScale());

	pBtnCancel = new QPushButton(this);
	pBtnCancel->setText(QString::fromLocal8Bit("取消"));
	pBtnCancel->setFixedSize(139 * getScale(), 51 * getScale());
	pBtnCancel->setStyleSheet(
		QString("QPushButton{border: %1px solid rgba(170,170,170); border-radius: %2;"
			"background-color: white; color: rgb(68, 68, 68); %3}")
			.arg(1 * getScale())
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	pBtnCancel->setCheckable(true);
	TestWidget *frame = new TestWidget(this);
	frame->setFixedSize(50, 50);
	frame->move(0, 0);
	frame->hide();
	connect(frame, &TestWidget::SignalWidgetHide, this, [=]() {
		pBtnCancel->setChecked(false);
	});
	connect(pBtnCancel, &QPushButton::clicked, this, [=](bool checked) {
		if (checked) {
			qDebug() << "-------" << this->x() << this->y()
				 << this->mapToGlobal(
					    QPoint(this->x(), this->y()));
			frame->move(this->mapToGlobal(
				QPoint(((QWidget *)this->parent())->x(),
				       ((QWidget *)this->parent())->y())));
			frame->show();
		}else
			frame->hide();
		//this->close();
		//this->deleteLater();
	});

	pBtnYes = new QPushButton(this);
	pBtnYes->setText(QString::fromLocal8Bit("确认添加"));
	pBtnYes->setFixedSize(139 * getScale(), 51 * getScale());
	pBtnYes->setStyleSheet(
		QString("QPushButton{background-color: rgba(46,105,240); border-radius: %1; "
			"color: rgb(255, 255, 255); %2}")
			.arg(6 * getScale())
			.arg(getFontStyle(16)));

	QHBoxLayout *layout_bottom = new QHBoxLayout(frame_bottom);
	layout_bottom->setSpacing(30 * getScale());
	layout_bottom->addSpacerItem(
		new QSpacerItem(1, 1, QSizePolicy::Expanding));
	layout_bottom->addWidget(pBtnCancel);
	layout_bottom->addWidget(pBtnYes);
	layout_bottom->addSpacerItem(
		new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);

	vlayout->addWidget(frame_top);
	vlayout->addWidget(frame_tab);
	vlayout->addWidget(stackedWidget, 0, Qt::AlignHCenter);
	vlayout->addWidget(frame_bottom);
	vlayout->addWidget(frame_bottom);

	QButtonGroup *buttongroup = new QButtonGroup(this);
	buttongroup->addButton(tabBtn_1);
	buttongroup->addButton(tabBtn_2);
	buttongroup->addButton(tabBtn_3);
	buttongroup->setExclusive(true);
	tabBtn_1->setChecked(true);

	sceneSettingsWidget = new SceneSettingsWidget(this->stackedWidget);
	ipCameraSettingsWidget =
		new IpCameraSettingsWidget(this->stackedWidget);
	usbCameraSettingsWidget =
		new USBCameraSettingsWidget(this->stackedWidget);
	this->stackedWidget->addWidget(sceneSettingsWidget);
	this->stackedWidget->addWidget(ipCameraSettingsWidget);
	this->stackedWidget->addWidget(usbCameraSettingsWidget);
	this->stackedWidget->setCurrentWidget(sceneSettingsWidget);

	this->setFixedSize(612 * getScale(), 496 * getScale());
}

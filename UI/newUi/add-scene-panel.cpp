#include "add-scene-panel.hpp"
#include <qbuttongroup.h>
#include "obs-app.hpp"

/********************************** ComboBoxItemWidget ***************************************************/
ComboBoxItemWidget::ComboBoxItemWidget(int index, int width, int height,
				       bool hideLine, QWidget *parent)
	: itemIndex(index), QWidget(parent)
{
	this->setFixedSize(width, height);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	int margin = 20 * getScale();
	pBtn_text = new QPushButton(this);
	pBtn_text->setCheckable(true);
	pBtn_text->setStyleSheet(
		QString("QPushButton{background-color: transparent; color: rgb(68, 68, 68); border:none; "
			"border-radius: none; text-align: left; padding-left: %1px; %2}"
			"QPushButton::checked {color: rgb(46, 105, 240);}"
			"QPushButton::hover{background-color: rgb(239, 243, 254);}"
			"QPushButton::disabled{text-align: center;}")
			.arg(margin)
			.arg(getFontStyle(16)));
	connect(pBtn_text, &QPushButton::clicked, this,
		[=]() { emit btnClicked(itemIndex, pBtn_text->text()); });

	QLabel *label_line = new QLabel(this);
	label_line->setFixedSize(width - margin * 2, 1 * getScale());
	if (!hideLine)
		label_line->setStyleSheet(
			"QLabel{background-color: rgb(224, 224, 224); }");
	else
		label_line->setStyleSheet(
			"QLabel{background-color: transparent;}");

	pBtn_text->setFixedHeight(height - label_line->height());
	layout->addWidget(pBtn_text);
	layout->addWidget(label_line, 0, Qt::AlignHCenter);
}

void ComboBoxItemWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QBrush(QColor(241, 58, 164, 0)));
	return QWidget::paintEvent(event);
}

/********************************** ComboBoxView ***************************************************/
ComboBoxView::ComboBoxView(QWidget *parent) : QWidget(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint |
			     Qt::WindowStaysOnTopHint | Qt::Popup |
			     Qt::NoDropShadowWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setAttribute(Qt::WA_NoMouseReplay, true);
	this->hide();

	shadowBorder = 4 * getScale();
	itemHeight = 54 * getScale();
	maxDisplayCount = 6;
	//this->setFixedHeight(0);

	listWidget = new QListWidget(this);
	listWidget->setStyleSheet(
		QString("QListWidget{border:none; background-color: transparent; padding: 0px; }"
			"QListWidget::item{padding: 0px;}"
			"QListWidget::Item:hover{background-color: transparent; }"
			"QScrollBar:vertical{width:%1px; background-color: rgb(170, 170, 170);}")
			.arg(6 * getScale()));
	listWidget->setContentsMargins(0, 0, 0, 0);
	listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listWidget->setMaximumHeight(itemHeight * maxDisplayCount);
	listWidget->setResizeMode(QListWidget::Adjust);
	listWidget->setFocusPolicy(Qt::NoFocus);
	listWidget->setGridSize(QSize(itemHeight, itemHeight));
	listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	listWidget->setSelectionMode(QAbstractItemView::NoSelection);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(shadowBorder, shadowBorder, shadowBorder,
				   shadowBorder);
	layout->addWidget(listWidget);

	itemGroup = new QButtonGroup(this);
	itemGroup->setExclusive(true);
}

void ComboBoxView::addItems(QStringList textList, QList<int> titleIndexList)
{
	if (itemMap.keys().size() > 0) {
		foreach(ComboBoxItemWidget * itemWidget, itemMap.values())
		{
			itemGroup->removeButton(itemWidget->getBtn());
			itemWidget->deleteLater();
		}
		int count = listWidget->count();
		for (int i = 0; i < count; i++) {
			QListWidgetItem *item = listWidget->takeItem(0);
			delete item;
		}
		itemMap.clear();
	}

	QAbstractButton *firstItem = nullptr;
	for (int i = 0; i < textList.size(); i++) {
		QListWidgetItem *item = new QListWidgetItem(listWidget);
		ComboBoxItemWidget *itemWidget = new ComboBoxItemWidget(
			i, this->width(), itemHeight,
			(i == textList.size() - 1) ? true : false, listWidget);
		connect(itemWidget, &ComboBoxItemWidget::btnClicked, this,
			[=](int index, QString text) {
				currentIndex = index;
				this->hide();
				emit itemIndexChanged(index, text);
			});
		itemWidget->setText(textList.at(i));
		listWidget->addItem(item);
		listWidget->setItemWidget(item, itemWidget);
		itemMap.insert(item, itemWidget);
		itemGroup->addButton(itemWidget->getBtn());
		foreach(int titleIndex, titleIndexList) {
			if (titleIndex == i)
				itemWidget->setBtnEnabled(false);
		}
		if (titleIndexList.size() > 0 && 1 == i)
			firstItem = itemWidget->getBtn();
		else if (i == 0)
			firstItem = itemWidget->getBtn();	
	}

	itemCount = textList.size();
	if (itemCount <= maxDisplayCount)
		this->setFixedHeight(itemHeight * itemCount + shadowBorder * 2);
	else
		this->setFixedHeight(itemHeight * maxDisplayCount +
				     shadowBorder * 2);

	if (nullptr != firstItem)
		firstItem->click();
}

void ComboBoxView::setMaxDisplayCount(int count)
{
	listWidget->setMaximumHeight(itemHeight * maxDisplayCount);
	maxDisplayCount = count;
}

void ComboBoxView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QColor color(135, 135, 135);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	int j = 1;
	for (int i = 0; i < shadowBorder; i++) {
		QRect rect = QRect(QPoint(shadowBorder - i, shadowBorder - i),
				   QPoint(this->width() - (shadowBorder - i),
					  this->height() - (shadowBorder - i)));
		QPainterPath path;
		int radius = 4 * getScale();
		path = getRoundRectPath(rect, radius, radius, radius, radius);
		path.setFillRule(Qt::WindingFill);

		if (6 == i) {
			switch (i) {
			case 0:
				color.setAlpha(50);
				break;
			case 1:
				color.setAlpha(25);
				break;
			case 2:
				color.setAlpha(17);
				break;
			case 3:
				color.setAlpha(13);
				break;
			case 4:
				color.setAlpha(8);
				break;
			case 5:
				color.setAlpha(3);
				break;
			default:
				break;
			}
		} else {
			j = 1 + i / 10 * 5;
			if (0 == i) {
				painter.fillPath(path, Qt::white);
			}
			color.setAlpha(50 / ((i + 1) * j));
		}

		pen.setColor(color);
		painter.setPen(pen);
		painter.drawPath(path);
	}
}

bool ComboBoxView::event(QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress) {
		return true;
	} else if (e->type() == QEvent::MouseButtonRelease) {
		this->hide();
		emit viewHide();
		return true;
	} else if (e->type() == QEvent::HideToParent) {
		emit viewHide();
	}

	return QWidget::event(e);
}

/********************************** ComboBox ***************************************************/
ComboBox::ComboBox(QWidget *parent) : QFrame(parent)
{
	this->setStyleSheet(
		QString("QFrame{background-color: rgb(240,240,240); border-radius: %1px;}")
			.arg(4 * getScale()));
	this->initUi();
}

void ComboBox::addItems(QStringList textList, QList<int> titleIndexList)
{
	if (textList.isEmpty())
		return;

	if (nullptr == view) {
		view = new ComboBoxView(this);
		connect(view, &ComboBoxView::itemIndexChanged, this,
			[=](int index, QString text) {
				m_Index = index;
				this->setText(text);
				emit itemIndexChanged(text);
			});
		view->setFixedWidth(this->width());

		connect(view, &ComboBoxView::viewHide, this, [=]() {
			if (this->checked) {
				checked = false;
				if (label_icon != nullptr)
					label_icon->setPixmap(QPixmap(
						":/res/images/newUi/arrow@2x.png"));
			}
		});
	}

	view->addItems(textList, titleIndexList);
}

bool ComboBox::event(QEvent *e)
{
	if (e->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(e);
		if (mouseevent->button() == Qt::LeftButton && nullptr != view) {
			checked = !checked;
			if (checked) {
				if (label_icon != nullptr)
					label_icon->setPixmap(QPixmap(
						":/res/images/newUi/arrow_on@2x.png"));
				if (view != nullptr) {
					QPoint globalPos =
						mapToGlobal(QPoint(0, 0));
					view->move(
						globalPos.x() - view->shadow(),
						globalPos.y() + this->height());
					view->show();
				}
			} else {
				if (label_icon != nullptr)
					label_icon->setPixmap(QPixmap(
						":/res/images/newUi/arrow@2x.png"));
				if (view != nullptr)
					view->hide();
			}
		}
	} else if (e->type() == QEvent::MouseButtonPress) {
	}

	return QFrame::event(e);
}

void ComboBox::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	if (nullptr != view)
		view->setFixedWidth(this->width());
}

void ComboBox::initUi()
{
	QHBoxLayout *hlayout = new QHBoxLayout(this);
	hlayout->setSpacing(0);
	hlayout->setContentsMargins(21 * getScale(), 0, 21 * getScale(), 0);
	label_text = new QLabel(this);
	label_text->setStyleSheet(QString("QLabel{color: rgb(68,68,68); %1}")
					  .arg(getFontStyle(18)));
	label_icon = new QLabel(this);
	label_icon->setFixedSize(15 * getScale(), 10 * getScale());
	label_icon->setPixmap(QPixmap(":/res/images/newUi/arrow@2x.png"));
	hlayout->addWidget(label_text, 1, Qt::AlignVCenter);
	hlayout->addWidget(label_icon, 0, Qt::AlignVCenter);
}

void ComboBox::setText(const QString &text)
{
	int label_width = this->width() - 57 * getScale();
	if (label_text != nullptr) {
		QFontMetrics fontMetric(getFont(18));
		int fontSize = fontMetric.width(text);
		QString str = text;
		if (fontSize > label_width)
			str = fontMetric.elidedText(text, Qt::ElideRight,
						    label_width);
		label_text->setText(str);
	}
}

/*********************************  SceneSettingsWidget ****************************************/
void SceneSettingsWidget::initData(
	std::vector<std::shared_ptr<accrecorder::source::ScreenSceneItem>>
		screenItems)
{
	QStringList sceneList;
	foreach(auto item, screenItems)
		sceneList.append(QString::fromStdString(item->Name()));

	combobox_scenes->addItems(sceneList);
	combobox_fps->addItems(QStringList() << "30Hz");
	combobox_encode->addItems(QStringList() << "H264");
	combobox_resolution->addItems(QStringList() << "1080p");
}

void SceneSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QTStr("NewUi.BlackBoardScreen"), this);
	label->setFixedWidth(72 * getScale());
	combobox_scenes = new ComboBox(this);
	combobox_scenes->setFixedSize(410 * getScale(), 51 * getScale());

	label->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
				     .arg(getFontStyle(18)));

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout->addWidget(label, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(
		new QSpacerItem(30 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(combobox_scenes, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_1 = new QLabel(QTStr("NewUi.Fps"), this);
	QLabel *label_2 = new QLabel(QTStr("NewUi.Encode"), this);
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

	QLabel *label_3 = new QLabel(QTStr("NewUi.Resolution"), this);
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

void IpCameraSettingsWidget::initData()
{
	QStringList ipCameraList = QStringList()
				   << QTStr("NewUi.TeacherCloseUp")
				   << QTStr("NewUi.TeacherPanorama")
				   << QTStr("NewUi.StudentCloseUp")
				   << QTStr("NewUi.StudentPanorama");

	combobox_cameraName->addItems(ipCameraList);
	combobox_rate->addItems(QStringList() << "4096kbit/s");
	combobox_fps->addItems(QStringList() << "30Hz");
	combobox_encode->addItems(QStringList() << "H264");
	combobox_resolution->addItems(QStringList() << "1080p");
}

void IpCameraSettingsWidget::setEmptyStyle()
{
	lineedit_rtsp->setStyleSheet(
		QString("QLineEdit{ background-color:rgb(240,240,240); border: 1px solid red; color: rgb(68, 68, 68);border-radius: %1px; %2}"
			"QLineEdit::hover{background-color: rgb(240,240,240);}"
			"QLineEdit::disabled{color: rgb(170, 170, 170);}")
			.arg(4 * getScale())
			.arg(getFontStyle(18)));
}

void IpCameraSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QTStr("NewUi.CameraName"), this);
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

	QLabel *label_1 = new QLabel("RTSP", this);
	label_1->setFixedWidth(label->width());
	label_1->setStyleSheet(label->styleSheet());
	lineedit_rtsp = new QLineEdit(this);
	lineedit_rtsp->setFixedSize(410 * getScale(), 51 * getScale());
	lineedit_rtsp->setStyleSheet(
		QString("QLineEdit{ background-color:rgb(240,240,240); border: none; color: rgb(68, 68, 68);border-radius: %1px; padding-left: %3px; %2}"
			"QLineEdit::hover{background-color: rgb(240,240,240);}"
			"QLineEdit::disabled{color: rgb(170, 170, 170);}")
			.arg(4 * getScale())
			.arg(getFontStyle(18))
			.arg(21 * getScale()));
	lineedit_rtsp->setPlaceholderText(QTStr("NewUi.Input") + "RTSP");
	lineedit_rtsp->setFont(getFont(18));
	QPalette palette = lineedit_rtsp->palette();
	palette.setColor(QPalette::Normal, QPalette::PlaceholderText,
			 QColor(170, 170, 170));
	lineedit_rtsp->setPalette(palette);
	lineedit_rtsp->setContextMenuPolicy(Qt::NoContextMenu);

	QHBoxLayout *hlayout_1 = new QHBoxLayout;
	hlayout_1->setContentsMargins(0, 0, 0, 0);
	hlayout_1->setSpacing(0);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout_1->addWidget(label_1, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout_1->addWidget(lineedit_rtsp, 0, Qt::AlignVCenter);
	hlayout_1->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_2 = new QLabel(QTStr("NewUi.Rate"), this);
	QLabel *label_3 = new QLabel(QTStr("NewUi.Fps"), this);
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

	QLabel *label_4 = new QLabel(QTStr("NewUi.Encode"), this);
	QLabel *label_5 = new QLabel(QTStr("NewUi.Resolution"), this);
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

void USBCameraSettingsWidget::initData(
	std::vector<std::shared_ptr<accrecorder::source::CameraSceneItem>>
		usbCameraSource)
{
	// use updateDataSourceIfNeed
	//QStringList ipCameraList;
	//foreach(auto item, usbCameraSource)
	//{
	//	ipCameraList.append(QString::fromStdString(item->Name()));
	//	mapCameraSource.insert(QString::fromStdString(item->Name()),
	//			       item);
	//}
	//combobox_cameraName->addItems(ipCameraList);

	//combobox_fps->addItems(QStringList() << "30Hz");
	//combobox_rate->addItems(QStringList() << "4093kbit/s");
	//combobox_encode->addItems(QStringList() << "H264");
}

accrecorder::source::CameraSceneItem *
USBCameraSettingsWidget::getCurrentCameraSource()
{
	if (usbCameraSource.size() <= 0 )
		return nullptr;

	return usbCameraSource[combobox_cameraName
				       ->currentIndex()].get();
}

void USBCameraSettingsWidget::updateDataSourceIfNeed(
	accrecorder::manager::OBSSourceManager *manager)
{
	if (mapCameraSource.empty() && manager != nullptr) {
		usbCameraSource = std::vector<
			std::shared_ptr<accrecorder::source::CameraSceneItem>>();
		manager->ListCameraItems(usbCameraSource);

		QStringList ipCameraList;
		foreach(auto item, usbCameraSource)
		{
			ipCameraList.append(
				QString::fromStdString(item->Name()));
			mapCameraSource.insert(
				QString::fromStdString(item->Name()), item);
		}
		// TODO: remove old Combox items

		combobox_cameraName->addItems(ipCameraList);

		combobox_fps->addItems(QStringList() << "30Hz");
		combobox_rate->addItems(QStringList() << "4093kbit/s");
		combobox_encode->addItems(QStringList() << "H264");
	}
}

void USBCameraSettingsWidget::initUi()
{
	QLabel *label = new QLabel(QTStr("NewUi.CameraName"), this);
	label->setFixedWidth(90 * getScale());
	label->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
				     .arg(getFontStyle(18)));
	combobox_cameraName = new ComboBox(this);
	combobox_cameraName->setFixedSize(410 * getScale(), 51 * getScale());
	connect(combobox_cameraName, &ComboBox::itemIndexChanged, this,
		&USBCameraSettingsWidget::slot_combobox_cameraName_changed);

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	hlayout->addWidget(label, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(
		new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(combobox_cameraName, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

	QLabel *label_1 = new QLabel(QTStr("NewUi.Rate"), this);
	QLabel *label_2 = new QLabel(QTStr("NewUi.Fps"), this);
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

	QLabel *label_3 = new QLabel(QTStr("NewUi.Encode"), this);
	QLabel *label_4 = new QLabel(QTStr("NewUi.Resolution"), this);
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

void USBCameraSettingsWidget::slot_combobox_cameraName_changed(QString text)
{
	std::shared_ptr<accrecorder::source::CameraSceneItem> itemSource =
		mapCameraSource.value(text, nullptr);
	if (nullptr == itemSource)
		return;

	std::vector<std::string> res;
	std::vector<std::tuple<std::string, int64_t>> fps;
	itemSource->GetAvailableResolutions(res);
	itemSource->GetAvailableFps(fps);

	QStringList temp;
	foreach(std::string str, res)
	{
		temp.append(QString::fromStdString(str));
	}
	combobox_resolution->addItems(temp);

	//temp.clear();
	//for (int i = 0; i < fps.size(); i ++) {
	//	std::tuple<std::string, int64_t> stuple = fps.at(i);
	//	qDebug() << "---0:" << QString::fromStdString(std::get<0>(stuple));
	//	qDebug() << "---1:" << std::get<1>(stuple);
	//}
}

/*********************************** Basic panel *******************************************/
void BasicPanel::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QColor color(135, 135, 135);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	int j = 1;
	for (int i = 0; i < shadowBorder; i++) {
		QRect rect = QRect(QPoint(shadowBorder - i, shadowBorder - i),
				   QPoint(this->width() - (shadowBorder - i),
					  this->height() - (shadowBorder - i)));
		QPainterPath path;
		int radius = 12 * getScale();
		path = getRoundRectPath(rect, radius, radius, radius, radius);
		path.setFillRule(Qt::WindingFill);

		if (0 == i)
			painter.fillPath(path, QColor(255, 255, 255));
		if (4 == shadowBorder) {
			switch (i) {
			case 0:
				color.setAlpha(65);
				break;
			case 1:
				color.setAlpha(25);
				break;
			case 2:
				color.setAlpha(13);
				break;
			case 3:
				color.setAlpha(3);
				break;
			default:
				break;
			}
		} else if (8 == shadowBorder) {
			switch (i) {
			case 0:
				color.setAlpha(65);
				break;
			case 1:
				color.setAlpha(35);
				break;
			case 2:
				color.setAlpha(25);
				break;
			case 3:
				color.setAlpha(13);
				break;
			case 4:
				color.setAlpha(10);
				break;
			case 5:
				color.setAlpha(8);
				break;
			case 6:
				color.setAlpha(5);
				break;
			case 7:
				color.setAlpha(3);
				break;
			default:
				break;
			}
		}

		pen.setColor(color);
		painter.setPen(pen);
		painter.drawPath(path);
	}
}

bool BasicPanel::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == this && event->type() == QEvent::MouseButtonPress) {
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
	} else if (obj != this && event->type() == QEvent::MouseMove) {
		return true;
	}
	return QWidget::eventFilter(obj, event);
}

/*********************************** Add scene panel *******************************************/
ScenesSettingsPanel::ScenesSettingsPanel(
	accrecorder::manager::OBSSourceManager *manager,
	accrecorder::source::SceneItem::Category categroy, QWidget *parent)
	: sourceManager(manager), itemCategory(categroy), BasicPanel(parent)
{	
	this->initUi();
	this->installEventFilter(this);
	this->initData();

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, [=]() {
		m_timer->stop();
		usbCameraSettingsWidget->updateDataSourceIfNeed(sourceManager);
		m_timer->deleteLater();
		m_timer = nullptr;
	});
}

void ScenesSettingsPanel::show()
{
	BasicPanel::show();
}

void ScenesSettingsPanel::initUi()
{
	frame_top = new QFrame(this);
	frame_top->setFixedHeight(70 * getScale());
	frame_top->setObjectName("frame_top");
	frame_top->setStyleSheet("#frame_top{background-color: transparent;}");

	QLabel *label_title = new QLabel(frame_top);
	if (itemCategory == accrecorder::source::SceneItem::Category::kMain)
		label_title->setText(QTStr("NewUi.AddMainScreen"));
	else if (itemCategory == accrecorder::source::SceneItem::Category::kPiP)
		label_title->setText(QTStr("NewUi.AddPip"));
	label_title->setStyleSheet(
		QString("QLabel{color:rgb(34,34,34); %1}")
			.arg(getFontStyle(22, FontWeight::Blod)));

	pBtn_close = new QPushButton(frame_top);
	pBtn_close->setFixedSize(36 * getScale(), 36 * getScale());
	pBtn_close->setStyleSheet(
		"QPushButton{padding: 0px; border: none; min-width: 36px; min-height: 36px;"
		"background-color: transparent; border-image: url(':/res/images/newUi/close@2x.png');}");
	connect(pBtn_close, &QPushButton::clicked, this, [=]() {
		this->close();
		this->deleteLater();
	});
	pBtn_close->installEventFilter(this);

	QHBoxLayout *layout_top = new QHBoxLayout(frame_top);
	layout_top->setContentsMargins(30 * getScale(), 0, 24 * getScale(), 0);
	layout_top->addWidget(label_title, 1, Qt::AlignVCenter);
	layout_top->addWidget(pBtn_close, 0, Qt::AlignVCenter);

	auto frame_tab = new QFrame(this);
	frame_tab->setFixedHeight(80 * getScale());

	tabBtn_1 = new QPushButton(QTStr("NewUi.Blackboard"), frame_tab);
	tabBtn_1->setCheckable(true);
	tabBtn_1->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_1->installEventFilter(this);
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

	tabBtn_2 = new QPushButton(("IP Camera"), frame_tab);
	tabBtn_2->setCheckable(true);
	tabBtn_2->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_2->installEventFilter(this);
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

	tabBtn_3 = new QPushButton(("USB Camera"), frame_tab);
	tabBtn_3->setCheckable(true);
	tabBtn_3->setFixedSize(168 * getScale(), 36 * getScale());
	tabBtn_3->installEventFilter(this);
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
		if (nullptr != m_timer)
			m_timer->start(100);
		//usbCameraSettingsWidget->updateDataSourceIfNeed(sourceManager);
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
	stackedWidget->installEventFilter(this);

	auto frame_bottom = new QFrame(this);
	frame_bottom->setFixedHeight(121 * getScale());

	pBtnCancel = new QPushButton(this);
	pBtnCancel->setText(QTStr("NewUi.Cancel"));
	pBtnCancel->setFixedSize(139 * getScale(), 51 * getScale());
	pBtnCancel->installEventFilter(this);
	pBtnCancel->setStyleSheet(
		QString("QPushButton{border: %1px solid rgb(170,170,170); border-radius: %2;"
			"background-color: white; color: rgb(68, 68, 68); %3}")
			.arg(1 * getScale())
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	pBtnCancel->setCheckable(true);
	connect(pBtnCancel, &QPushButton::clicked, this, [=](bool checked) {
		this->close();
		this->deleteLater();
	});

	pBtnYes = new QPushButton(this);
	pBtnYes->setText(QTStr("NewUi.ConfirmAdd"));
	pBtnYes->setFixedSize(139 * getScale(), 51 * getScale());
	pBtnYes->installEventFilter(this);
	pBtnYes->setStyleSheet(
		QString("QPushButton{background-color: rgb(46,105,240); border-radius: %1; "
			"color: rgb(255, 255, 255); %2}")
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	connect(pBtnYes, &QPushButton::clicked, this,
		&ScenesSettingsPanel::slot_addBtn_clicked);

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

void ScenesSettingsPanel::initData()
{
	sceneItems = std::vector<
			std::shared_ptr<accrecorder::source::ScreenSceneItem>>();
	//cameraItems = std::vector<
	//		std::shared_ptr<accrecorder::source::CameraSceneItem>>();
	sourceManager->ListScreenItems(sceneItems);
	//sourceManager->ListCameraItems(cameraItems);

	if (nullptr != sceneSettingsWidget)
		sceneSettingsWidget->initData(sceneItems);
	if (nullptr != ipCameraSettingsWidget)
		ipCameraSettingsWidget->initData();
	//if (nullptr != usbCameraSettingsWidget)
	//	usbCameraSettingsWidget->initData(cameraItems);
}

void ScenesSettingsPanel::slot_addBtn_clicked()
{
	accrecorder::source::SceneItem *screen = nullptr;
	if (0 == stackedWidget->currentIndex()) { //scene
		screen = new accrecorder::source::ScreenSceneItem(
			*sceneItems[sceneSettingsWidget->getSceneIndex()].get());
	} else if (1 == stackedWidget->currentIndex()) { // RTSP camera
		QString strRtsp = QString::fromStdString(ipCameraSettingsWidget->getRTSPURL());
		if (strRtsp.isEmpty()) {
			ipCameraSettingsWidget->setEmptyStyle();
			return;
		}
		
		screen = new accrecorder::source::IPCameraSceneItem(
			ipCameraSettingsWidget->getCameraName(),
			ipCameraSettingsWidget->getRTSPURL(), true);
	} else if (2 == stackedWidget->currentIndex()) { //usb camera
		accrecorder::source::CameraSceneItem *cameraSource =
			usbCameraSettingsWidget->getCurrentCameraSource();
		if (cameraSource == nullptr)
			return; 
		screen = new accrecorder::source::CameraSceneItem(
			*usbCameraSettingsWidget->getCurrentCameraSource());
	}

	if (screen == nullptr)
		return;

	if (sourceManager->AttachSceneItem(screen, itemCategory)) {
		if (itemCategory ==
		    accrecorder::source::SceneItem::Category::kMain)
			screen->Lock(true);
		else
			screen->Lock(false);
		// update settings
		screen->Hide(true);

		// update scale
		vec2 size = screen->OrignalSize();
		if (itemCategory ==
		    accrecorder::source::SceneItem::Category::kMain) {
			// scale to fit size: 1920*1080
			if (size.x != 1920 || size.y != 1080) {
				screen->UpdateScale(
					{1920.f / size.x, 1080.0f / size.y});
			}
		} else {
			// make 1/3 scale of the canvans
			screen->UpdateScale({640.f / size.x, 360.f / size.y});
		}

		if (sourceManager->ApplySceneItemSettingsUpdate(screen))
			emit attachFinished(screen, itemCategory);
	} else {
		// delete it if unable to attach to the scene
		delete screen;
		screen = nullptr;
	}

	this->close();
	this->deleteLater();
}

/*********************************** Add audio panel *******************************************/
AudioSettingsPanel::AudioSettingsPanel(
	accrecorder::manager::OBSSourceManager *manager, QWidget *parent)
	: sourceManager(manager), BasicPanel(parent)
{
	this->installEventFilter(this);
	this->initUi();
	this->initData();
	this->setObjectName("testaudiopanel");
}

void AudioSettingsPanel::initUi()
{
	QLabel *label_title = new QLabel(this);
	label_title->setText(QTStr("NewUi.AudioSettings"));

	label_title->setStyleSheet(
		QString("QLabel{color:rgb(34,34,34); %1}")
			.arg(getFontStyle(22, FontWeight::Blod)));
	label_title->setFixedSize(89 * getScale(), 23 * getScale());
	label_title->move(30 * getScale(), 24 * getScale());

	QPushButton *pBtn_close = new QPushButton(this);
	pBtn_close->setFixedSize(36 * getScale(), 36 * getScale());
	pBtn_close->move(440 * getScale(), 17 * getScale());
	pBtn_close->setStyleSheet(
		"QPushButton{padding: 0px; border: none; min-width: 36px; min-height: 36px;"
		"background-color: transparent; border-image: url(':/res/images/newUi/close@2x.png');}");
	connect(pBtn_close, &QPushButton::clicked, this, [=]() {
		this->close();
		this->deleteLater();
	});
	pBtn_close->installEventFilter(this);

	combobox_audio = new ComboBox(this);
	combobox_audio->setFixedSize(440 * getScale(), 51 * getScale());
	combobox_audio->move(30 * getScale(), 86 * getScale());
	//connect(combobox_autio, &ComboBox::itemIndexChanged, this, [=](){});

	QPushButton *pBtnCancel = new QPushButton(this);
	pBtnCancel->setText(QTStr("NewUi.Cancel"));
	pBtnCancel->setFixedSize(120 * getScale(), 40 * getScale());
	pBtnCancel->move(120 * getScale(), 177 * getScale());
	pBtnCancel->installEventFilter(this);
	pBtnCancel->setStyleSheet(
		QString("QPushButton{border: %1px solid rgb(170,170,170); border-radius: %2;"
			"background-color: white; color: rgb(68, 68, 68); %3}")
			.arg(1 * getScale())
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	connect(pBtnCancel, &QPushButton::clicked, this, [=]() {
		this->close();
		this->deleteLater();
	});

	QPushButton *pBtnYes = new QPushButton(this);
	pBtnYes->setText(QTStr("NewUi.Confirm"));
	pBtnYes->setFixedSize(pBtnCancel->width(), pBtnCancel->height());
	pBtnYes->move(260 * getScale(), pBtnCancel->y());
	pBtnYes->installEventFilter(this);
	pBtnYes->setStyleSheet(
		QString("QPushButton{background-color: rgb(46,105,240); border-radius: %1; "
			"color: rgb(255, 255, 255); %2}")
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	connect(pBtnYes, &QPushButton::clicked, this, [=]() {
		// copy
		auto input = new accrecorder::source::AudioInputItem(
			*reinterpret_cast<accrecorder::source::AudioInputItem *>(
				allAudioItems[combobox_audio->currentIndex()].get()));
		if (!sourceManager->AttachSceneItem(input)) {
			// failed
			delete input;
			input = nullptr;
		}
		this->close();
		this->deleteLater();
	});
}

void AudioSettingsPanel::initData()
{
	std::vector<std::shared_ptr<accrecorder::source::AudioSceneItem>>
		audioInuputItems = std::vector<std::shared_ptr<accrecorder::source::AudioSceneItem>>();
	std::vector<std::shared_ptr<accrecorder::source::AudioSceneItem>>
		audioOutputItems = std::vector<std::shared_ptr<accrecorder::source::AudioSceneItem>>();
	
	sourceManager->ListAudioItems(audioInuputItems);
	sourceManager->ListAudioItems(audioOutputItems, false);
	QStringList nameList;
	QList<int> titleIndexList;

	if (audioInuputItems.size() > 0) {
		titleIndexList.append(0);
		nameList.append(QTStr("NewUi.AudioInput"));
		allAudioItems.append(nullptr);
	}	
	foreach(auto item, audioInuputItems)
	{
		nameList.append(QString::fromStdString(item->Name()));
		allAudioItems.append(item);
	}
	if (audioOutputItems.size() > 0) {
		titleIndexList.append(nameList.size());
		nameList.append(QTStr("NewUi.AudioOutput"));
		allAudioItems.append(nullptr);
	}
	foreach(auto item, audioOutputItems)
	{
		nameList.append(QString::fromStdString(item->Name()));
		allAudioItems.append(item);
	}

	combobox_audio->addItems(nameList, titleIndexList);
}

/*********************************** Push stream panel *******************************************/
StreamingSettingsPanel::StreamingSettingsPanel(
	accrecorder::manager::OBSSourceManager *manager, QWidget *parent)
	: sourceManager(manager), BasicPanel(parent)
{
	this->installEventFilter(this);
	this->initUi();
	this->initData();
}

void StreamingSettingsPanel::initUi()
{
	QLabel *label_title = new QLabel(this);
	label_title->setText(QTStr("NewUi.StreamingSettings"));
	label_title->setStyleSheet(
		QString("QLabel{color:rgb(34,34,34); %1}")
			.arg(getFontStyle(22, FontWeight::Blod)));
	label_title->setFixedSize(89 * getScale(), 23 * getScale());
	label_title->move(30 * getScale(), 24 * getScale());

	QPushButton *pBtn_close = new QPushButton(this);
	pBtn_close->setFixedSize(36 * getScale(), 36 * getScale());
	pBtn_close->move(340 * getScale(), 17 * getScale());
	pBtn_close->setStyleSheet(
		"QPushButton{padding: 0px; border: none; min-width: 36px; min-height: 36px;"
		"background-color: transparent; border-image: url(':/res/images/newUi/close@2x.png');}");
	connect(pBtn_close, &QPushButton::clicked, this, [=]() {
		this->close();
		this->deleteLater();
	});
	pBtn_close->installEventFilter(this);

	QLabel *label_rtsp1 = new QLabel(this);
	label_rtsp1->setText("RTMP1");
	label_rtsp1->setFixedSize(70 * getScale(), 15 * getScale());
	label_rtsp1->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
					   .arg(getFontStyle(18)));
	label_rtsp1->move(31 * getScale(), 87 * getScale());

	QLabel *label_rtsp2 = new QLabel(this);
	label_rtsp2->setText("RTMP2");
	label_rtsp2->setFixedSize(70 * getScale(), 15 * getScale());
	label_rtsp2->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
					   .arg(getFontStyle(18)));
	label_rtsp2->move(31 * getScale(), 205 * getScale());

	QLabel *label_rtsp3 = new QLabel(this);
	label_rtsp3->setText("RTMP3");
	label_rtsp3->setFixedSize(70 * getScale(), 15 * getScale());
	label_rtsp3->setStyleSheet(QString("QLabel{color:rgb(34,34,34); %1}")
					   .arg(getFontStyle(18)));
	label_rtsp3->move(31 * getScale(), 323 * getScale());

	lineedit_rtsp1 = new QLineEdit(this);
	lineedit_rtsp1->setFixedSize(340 * getScale(), 51 * getScale());
	lineedit_rtsp1->setEnabled(true);
	lineedit_rtsp1->setStyleSheet(
		QString("QLineEdit{ background-color:rgb(240,240,240); border: none; color: rgb(68, 68, 68);border-radius: %1px; padding-left: %3px; %2}"
			"QLineEdit::hover{background-color: rgb(240,240,240);}"
			"QLineEdit::disabled{color: rgb(170, 170, 170);}")
			.arg(4 * getScale())
			.arg(getFontStyle(18))
			.arg(21 * getScale()));
	lineedit_rtsp1->setPlaceholderText(QTStr("NewUi.Input") + "RTMP1");
	lineedit_rtsp1->move(30 * getScale(), 123 * getScale());
	QPalette palette = lineedit_rtsp1->palette();
	palette.setColor(QPalette::PlaceholderText, QColor(170, 170, 170));
	lineedit_rtsp1->setPalette(palette);
	lineedit_rtsp1->setContextMenuPolicy(Qt::NoContextMenu);

	lineedit_rtsp2 = new QLineEdit(this);
	lineedit_rtsp2->setFixedSize(340 * getScale(), 51 * getScale());
	lineedit_rtsp2->setEnabled(false);
	lineedit_rtsp2->setStyleSheet(
		QString("QLineEdit{ background-color:rgb(240,240,240); border: none; color: rgb(68, 68, 68);border-radius: %1px;  padding-left: %3px; %2}"
			"QLineEdit::hover{background-color: rgb(240,240,240);}"
			"QLineEdit::disabled{color: rgb(170, 170, 170);}")
			.arg(4 * getScale())
			.arg(getFontStyle(18))
			.arg(21 * getScale()));
	lineedit_rtsp2->setPlaceholderText(QTStr("NewUi.Input") + "RTMP2");
	lineedit_rtsp2->setFont(getFont(18));
	lineedit_rtsp2->move(30 * getScale(), 241 * getScale());
	lineedit_rtsp2->setPalette(lineedit_rtsp1->palette());
	lineedit_rtsp2->setContextMenuPolicy(Qt::NoContextMenu);

	lineedit_rtsp3 = new QLineEdit(this);
	lineedit_rtsp3->setFixedSize(340 * getScale(), 51 * getScale());
	lineedit_rtsp3->setEnabled(false);
	lineedit_rtsp3->setStyleSheet(
		QString("QLineEdit{ background-color:rgb(240,240,240); border: none; color: rgb(68, 68, 68);border-radius: %1px;  padding-left: %3px; %2}"
			"QLineEdit::hover{background-color: rgb(240,240,240);}"
			"QLineEdit::disabled{color: rgb(170, 170, 170);}")
			.arg(4 * getScale())
			.arg(getFontStyle(18))
			.arg(21 * getScale()));
	lineedit_rtsp3->setPlaceholderText(QTStr("NewUi.Input") + "RTMP3");
	lineedit_rtsp3->setFont(getFont(18));
	lineedit_rtsp3->move(30 * getScale(), 359 * getScale());
	lineedit_rtsp3->setPalette(lineedit_rtsp1->palette());
	lineedit_rtsp3->setContextMenuPolicy(Qt::NoContextMenu);

	QCheckBox *checkbox_rtsp1 = new QCheckBox(this);
	checkbox_rtsp1->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_rtsp1->move(324 * getScale(), 79 * getScale());
	checkbox_rtsp1->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_rtsp1->width())
			.arg(checkbox_rtsp1->height()));
	checkbox_rtsp1->setChecked(true);
	connect(checkbox_rtsp1, &QCheckBox::stateChanged, this, [=](int state) {
		if (0 == state)
			lineedit_rtsp1->setEnabled(false);
		else if (2 == state)
			lineedit_rtsp1->setEnabled(true);
	});

	QCheckBox *checkbox_rtsp2 = new QCheckBox(this);
	checkbox_rtsp2->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_rtsp2->move(324 * getScale(), 197 * getScale());
	checkbox_rtsp2->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_rtsp2->width())
			.arg(checkbox_rtsp2->height()));
	connect(checkbox_rtsp2, &QCheckBox::stateChanged, this, [=](int state) {
		if (0 == state)
			lineedit_rtsp2->setEnabled(false);
		else if (2 == state)
			lineedit_rtsp2->setEnabled(true);
	});

	QCheckBox *checkbox_rtsp3 = new QCheckBox(this);
	checkbox_rtsp3->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_rtsp3->setFixedSize(42 * getScale(), 24 * getScale());
	checkbox_rtsp3->move(324 * getScale(), 315 * getScale());
	checkbox_rtsp3->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/switch@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/switch2@2x.png');}")
			.arg(checkbox_rtsp3->width())
			.arg(checkbox_rtsp3->height()));
	connect(checkbox_rtsp3, &QCheckBox::stateChanged, this, [=](int state) {
		if (0 == state)
			lineedit_rtsp3->setEnabled(false);
		else if (2 == state)
			lineedit_rtsp3->setEnabled(true);
	});

	QButtonGroup *group = new QButtonGroup(this);
	group->setExclusive(true);
	group->addButton(checkbox_rtsp1);
	group->addButton(checkbox_rtsp2);
	group->addButton(checkbox_rtsp3);

	QPushButton *pBtnCancel = new QPushButton(this);
	pBtnCancel->setText(QTStr("NewUi.Cancel"));
	pBtnCancel->setFixedSize(120 * getScale(), 40 * getScale());
	pBtnCancel->move(70 * getScale(), 450 * getScale());
	pBtnCancel->installEventFilter(this);
	pBtnCancel->setStyleSheet(
		QString("QPushButton{border: %1px solid rgb(170,170,170); border-radius: %2;"
			"background-color: white; color: rgb(68, 68, 68); %3}")
			.arg(1 * getScale())
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	connect(pBtnCancel, &QPushButton::clicked, this, [=]() {
		this->close();
		this->deleteLater();
	});

	QPushButton *pBtnYes = new QPushButton(this);
	pBtnYes->setText(QTStr("NewUi.Confirm"));
	pBtnYes->setFixedSize(pBtnCancel->width(), pBtnCancel->height());
	pBtnYes->move(210 * getScale(), pBtnCancel->y());
	pBtnYes->installEventFilter(this);
	pBtnYes->setStyleSheet(
		QString("QPushButton{background-color: rgb(46,105,240); border-radius: %1; "
			"color: rgb(255, 255, 255); %2}")
			.arg(6 * getScale())
			.arg(getFontStyle(16)));
	connect(pBtnYes, &QPushButton::clicked, this, [=]() {
		QString str;
		if (checkbox_rtsp1->isChecked())
			str = lineedit_rtsp1->text();
		else if (checkbox_rtsp2->isChecked())
			str = lineedit_rtsp2->text();
		else if (checkbox_rtsp3->isChecked())
			str = lineedit_rtsp3->text();

		if (str.isEmpty())
			return;

		sourceManager->SetStreamAddress(str.toStdString(),
						QString().toStdString(),
						QString().toStdString());
		this->close();
		this->deleteLater();
	});
}

void StreamingSettingsPanel::initData()
{
	std::string address;
	sourceManager->GetSteamAddress(address, QString().toStdString(),
				       QString().toStdString());
	lineedit_rtsp1->setText(QString::fromStdString(address));
}

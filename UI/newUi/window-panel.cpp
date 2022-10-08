#include "window-panel.hpp"

#include <qpushbutton.h>
#include <qscrollbar.h>
#include "font.hpp"

OBSPanelItem::OBSPanelItem(int type, const QString &name, int index, QWidget *parent)
	: m_type(type), m_name(name), m_index(index), QWidget(parent)
{
	this->installEventFilter(this);
	this->initUi();
}

void OBSPanelItem::initUi()
{
	QHBoxLayout *hlayout = new QHBoxLayout(this);
	hlayout->setContentsMargins(16 * getScale(), 0, 16 * getScale(), 0);
	hlayout->setSpacing(0);

	label_icon = new QLabel(this);
	label_icon->setFixedSize(32 * getScale(), 32 * getScale());

	frame = new QFrame(this);
	frame->setObjectName("frame");
	frame->setStyleSheet(QString("#frame{ background-color: transparent; border-radius: 0px; "
		"border: 1px solid transparent; border-top-color: %1}").arg(0 == m_index ? "rgb(85, 85, 85, 0);" : "rgb(85, 85, 85);"));

	hlayout->addWidget(label_icon, 0, Qt::AlignVCenter);
	hlayout->addSpacerItem(new QSpacerItem(16 * getScale(), 1, QSizePolicy::Fixed));
	hlayout->addWidget(frame, 0, Qt::AlignVCenter);


	QHBoxLayout *frame_layout = new QHBoxLayout(frame);
	frame_layout->setContentsMargins(0, 0, 0, 0);
	frame_layout->setSpacing(0);

	label_name = new QLabel(frame);
	label_name->setText(m_name);
	label_name->setStyleSheet(QString("QLabel{color:rgb(0,184,169); %1}"
					"QLabel::disabled{color: rgb(255, 255, 255);}").arg(getFontStyle(14)));

	checkbox = new QCheckBox(frame);
	checkbox->setFixedSize(16 * getScale(), 16 * getScale());
	checkbox->setStyleSheet(
		QString("QCheckBox { background-color: transparent; border: none;} "
			"QCheckBox::indicator { width: %1; height:%2; image: url(''); }"
			"QCheckBox::indicator:unchecked { border-image: url(':/res/images/newUi/danxuan@2x.png');}"
			"QCheckBox::indicator:checked { border-image: url(':/res/images/newUi/danxuan2@2x.png');}")
			.arg(checkbox->width())
			.arg(checkbox->height()));
	connect(checkbox, &QCheckBox::stateChanged, this, [=](int state) {
		bool checked = false;
		if (2 == state)
			checked = true;

		this->setIcon(checked);
		emit itemCheckedStateChanged(m_index, checked);
	});

	//QPushButton *pBtn_edit = new QPushButton(frame);
	//pBtn_edit->setFixedSize(24 * getScale(), 24 * getScale());
	//pBtn_edit->setStyleSheet(QString("QPushButton{ border-image: url(':/res/images/newUi/more@2x.png'); "
	//					"min-width: %1; min-height:%2px; padding:0px; border: 1px solid transparent; background-color: transparent;}")
	//					.arg(34 * getScale()).arg(18 * getScale()));
	//connect(pBtn_edit, &QPushButton::clicked, this, [=]() {  });

	QPushButton *pBtn_delete = new QPushButton(frame);
	pBtn_delete->setFixedSize(16 * getScale(), 16 * getScale());
	pBtn_delete->setStyleSheet(QString("QPushButton{ border-image: url(':/res/images/newUi/delete@2x.png'); "
			"min-width: %1; min-height:%2px; padding:0px; border: 1px solid transparent; background-color: transparent;}"
			"QPushButton::hover{ border-image: url(':/res/images/newUi/delete2@2x.png');}")
			.arg(16 * getScale()).arg(16 * getScale()));
	connect(pBtn_delete, &QPushButton::clicked, this,[=]() { emit deleteItem(m_index); });

	frame_layout->addWidget(label_name, 0, Qt::AlignVCenter);
	frame_layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	frame_layout->addWidget(checkbox, 0, Qt::AlignVCenter);
	frame_layout->addSpacerItem(new QSpacerItem(12 * getScale(), 1, QSizePolicy::Fixed));
	//frame_layout->addWidget(pBtn_edit, 0, Qt::AlignVCenter);
	//frame_layout->addSpacerItem(new QSpacerItem(16 * getScale(), 1, QSizePolicy::Fixed));
	frame_layout->addWidget(pBtn_delete, 0, Qt::AlignVCenter);

	this->setIcon(false);
}

void OBSPanelItem::setIcon(bool checked)
{
	if (0 == m_type) {
		if (!checked)
			label_icon->setPixmap(QPixmap(":/res/images/newUi/banshu@2x.png"));
		else
			label_icon->setPixmap(QPixmap(":/res/images/newUi/banshu2@2x.png"));
	} else if (1 == m_type) {
		if (!checked)
			label_icon->setPixmap(QPixmap(":/res/images/newUi/ipcamera@2x.png"));
		else
			label_icon->setPixmap(QPixmap(":/res/images/newUi/ipcamera2@2x.png"));
	} else if (2 == m_type) {
		if (!checked)
			label_icon->setPixmap(QPixmap(":/res/images/newUi/usbcamera@2x.png"));
		else
			label_icon->setPixmap(QPixmap(":/res/images/newUi/usbcamera2@2x.png"));
	}
	if (nullptr != label_name)
		label_name->setEnabled(checked);
}

void OBSPanelItem::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), QBrush(QColor(68, 68, 68)));
}

bool OBSPanelItem::event(QEvent *e)
{
	if (e->type() == QEvent::Resize && nullptr != frame)
		frame->setFixedHeight(this->height());
		
	return QWidget::event(e);
}

OBSPanel::OBSPanel(const QString &title, QWidget *parent)
	: m_title(title), QWidget(parent)
{
	this->installEventFilter(this);
	this->initUi();
	
	checkBoxGroup = new QButtonGroup(this);
	checkBoxGroup->setExclusive(true);
}

void OBSPanel::addItem(accrecorder::source::SceneItem *item)
{
	sceneItemList.append(item);
	int type = 0;
	if (item->type() == accrecorder::source::SceneItem::Type::kScreen)
		type = 0;
	else if (item->type() ==accrecorder::source::SceneItem::Type::kIPCamera)
		type = 1;
	else if (item->type() == accrecorder::source::SceneItem::Type::kCamera)
		type = 2;

	OBSPanelItem *panelItem = new OBSPanelItem(type, QString::fromStdString(item->Name()), itemCount, this);
	panelItem->setFixedHeight(56 * getScale());
	panelItemList.append(panelItem);

	connect(panelItem, &OBSPanelItem::itemCheckedStateChanged, this, [=](int index, bool checked) {
		if (0 <= index && index < sceneItemList.size()) {
			accrecorder::source::SceneItem *sceneItem = sceneItemList[index];
			sceneItem->Hide(!checked);
			if (sceneItem->category() == accrecorder::source::SceneItem::Category::kMain)
				sceneItem->Lock(true);
			else if (sceneItem->category() == accrecorder::source::SceneItem::Category::kPiP)
				sceneItem->Lock(false);
			sourceManager->ApplySceneItemSettingsUpdate(sceneItem);
		}
	});

	connect(panelItem, &OBSPanelItem::deleteItem, this, [=](int index){
		accrecorder::source::SceneItem *item = sceneItemList[index];
		sceneItemList.removeOne(item);
		sourceManager->Remove(item);

		OBSPanelItem *pitem = panelItemList[index];
		panelItemList.removeOne(pitem);
		contentWidgetLayout->removeWidget(pitem);
		itemCount--;

		scrollAreaContentWidget->setFixedHeight(panelItem->height() * itemCount + contentWidgetLayout->spacing() * (itemCount - 1));
		hscrollArea->verticalScrollBar()->setValue(scrollAreaContentWidget->height());

		checkBoxGroup->removeButton(pitem->getCheckButton());
		pitem->deleteLater();

		for (int i = 0; i < itemCount; i++) {	//Reorder
			panelItemList[i]->setIndex(i);
		}
	});

	contentWidgetLayout->insertWidget(itemCount, panelItem);
	checkBoxGroup->addButton(panelItem->getCheckButton());
	contentWidgetLayout->setStretch(contentWidgetLayout->count() - 1, 1);

	itemCount++;
	scrollAreaContentWidget->setFixedHeight(panelItem->height() * itemCount + contentWidgetLayout->spacing() * (itemCount - 1));
	hscrollArea->verticalScrollBar()->setValue(scrollAreaContentWidget->height());

	if (!item->GetSettings().hidden)
		panelItem->getCheckButton()->setChecked(true);
}

void OBSPanel::addAudioItem(VolControl *item)
{
	foreach(auto volItem, volItemList) {
		if (volItem->GetName().compare(item->GetName()) == 0)
			return;
	}
	volItemList.append(item);

	connect(item, &VolControl::ConfigClicked, this, [=]() {
		VolControl *volItem = static_cast<VolControl*>(sender());
		contentWidgetLayout->removeWidget(volItem);
		itemCount--;

		scrollAreaContentWidget->setFixedHeight(item->height() * itemCount + contentWidgetLayout->spacing() * (itemCount - 1));
		hscrollArea->verticalScrollBar()->setValue(scrollAreaContentWidget->height());

		volItemList.removeOne(volItem);

		volItem->deleteLater();
	});

	
	item->setFixedHeight(105 * getScale());
	
	contentWidgetLayout->insertWidget(itemCount, item);
	contentWidgetLayout->setStretch(contentWidgetLayout->count() - 1, 1);

	itemCount++;
	scrollAreaContentWidget->setFixedHeight(item->height() * itemCount + contentWidgetLayout->spacing() * (itemCount - 1));
	hscrollArea->verticalScrollBar()->setValue(scrollAreaContentWidget->height());

}

void OBSPanel::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.fillRect(this->rect(), QColor(68, 68, 68, 0));
	return QWidget::paintEvent(event);
}

bool OBSPanel::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == hscrollArea && event->type() == QEvent::Resize) {
		scrollAreaContentWidget->setFixedWidth(hscrollArea->width());
	} 
	return QWidget::eventFilter(obj, event);
}

void OBSPanel::initUi()
{
	QFrame *frame_top = new QFrame(this);
	frame_top->setStyleSheet("QFrame{background-color: '#444444'; border-radius: 0px;}");
	frame_top->setFixedHeight(40 * getScale());

	QLabel *label_title = new QLabel(frame_top);
	label_title->setText(m_title);
	label_title->setStyleSheet(QString("QLabel{ color: white; %1}").arg(getFontStyle(14)));

	QPushButton *pBtn_add = new QPushButton(frame_top);
	pBtn_add->setFixedSize(24 * getScale(), 24 * getScale());
	pBtn_add->setStyleSheet(QString("QPushButton{ border-image: url(':/res/images/newUi/tianjia@2x.png'); "
						"min-width: %1; min-height:%1px; padding:0px; border: none; background-color: transparent;}").arg(24 * getScale()));
	connect(pBtn_add, &QPushButton::clicked, this, [=](){ emit addClicked(); });

	auto layout_top = new QHBoxLayout(frame_top);
	layout_top->setContentsMargins(16 * getScale(), 0, 16 * getScale(), 0);
	layout_top->addWidget(label_title, 1);
	layout_top->addWidget(pBtn_add, 0, Qt::AlignVCenter);

	listFrame = new QFrame();
	listFrame->setObjectName("listFrame");
	listFrame->setStyleSheet("#listFrame{background-color: rgb(68, 68, 68); border-radius: 0px;}");
	listFrame->setMinimumHeight(199 * getScale());

	QHBoxLayout *frame_layout = new QHBoxLayout(listFrame);
	frame_layout->setContentsMargins(0, 0, 0, 0);
	frame_layout->setSpacing(0);

	scrollAreaContentWidget = new QWidget(this);
	scrollAreaContentWidget->setFixedSize(0, 0);
	contentWidgetLayout = new QVBoxLayout(scrollAreaContentWidget);
	contentWidgetLayout->setContentsMargins(0, 0, 0, 0);
	contentWidgetLayout->setSpacing(0);
	contentWidgetLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
	contentWidgetLayout->setDirection(QBoxLayout::TopToBottom);
	QPalette palette(scrollAreaContentWidget->palette());
	palette.setBrush(QPalette::Background, QBrush(QColor(68, 68, 68, 0)));
	scrollAreaContentWidget->setPalette(palette);

	hscrollArea = new HScrollArea(this);
	hscrollArea->setWidgetResizable(true);
	hscrollArea->setStyleSheet("QScrollArea{border:none; background-color: rgb(68, 68, 68, 0); border-radius: 0px;}");
	hscrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	hscrollArea->setWidget(scrollAreaContentWidget);
	hscrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	hscrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	hscrollArea->installEventFilter(this);
	QScrollBar *scrBar = hscrollArea->verticalScrollBar();
	scrBar->setFixedWidth(6 * getScale());
	scrBar->setStyleSheet(
		QString("QScrollBar:vertical{"
			"width:%1px;background-color:#696969;"
			"margin:0px,0px,0px,0px;"
			"padding-top:0px;padding-bottom:0px;padding-right:0px;}"
			"QScrollBar::handle:vertical{"
			"border-radius: %2px;width:%3px;"
			"background: #696969;min-height:%4px;}"
			"QScrollBar::handle:vertical:hover{"
			"border-radius: %2px;width:%3px;"
			"background: #696969;min-height:%4px;}"
			"QScrollBar::handle:vertical:pressed{"
			"border-radius: %2px;width:%3px;"
			"background:#696969;min-height:%4px;}"
			"QScrollBar::sub-line:vertical, QScrollBar::add-line:vertical"
			"{width: 0px;height: 0px; background:transparent}"
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{background: transparent;}")
			.arg(6 * getScale())
			.arg(int(3 * getScale()))
			.arg(6 * getScale())
			.arg(int(0 * getScale())));

	frame_layout->addWidget(hscrollArea);


	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(1 * getScale());
	layout->addWidget(frame_top, 0);
	layout->addWidget(listFrame, 1);
	layout->setStretch(1, 1);
}


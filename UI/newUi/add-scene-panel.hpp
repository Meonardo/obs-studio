#pragma once

#include <QWidget>
#include <qlabel.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qguiapplication.h>
#include <qscreen.h>
#include <qlistwidget.h>
#include <qstackedwidget.h>
#include <qevent.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qitemdelegate.h>
#include <qabstractitemview.h>
#include <qscrollbar.h>
#include <qframe.h>
//#include <qkeyeventtransition.h>
#include "font.hpp"

class TestWidget : public QWidget
{
	Q_OBJECT
public:
	TestWidget(QWidget *parent = nullptr) : QWidget(parent) {
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
		//this->setAttribute(Qt::WA_NoMouseReplay);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		QPainter painter(this);
		painter.fillRect(this->rect(), QBrush(QColor(255, 0, 0)));
	}
	bool event(QEvent *e) override
	{
		if (e->type() == QEvent::HideToParent) {
			emit SignalWidgetHide();
		}
		//if (QEvent::Show == e->type()) {
		//	activateWindow();
		//} else if (QEvent::WindowDeactivate == e->type()) {
		//	//this->close();
		//}
		return QWidget::event(e);
	}

signals:
	void SignalWidgetHide();
};

class ComboBox : public QPushButton
{
	Q_OBJECT
public:
	explicit ComboBox(const QString text = QString(), QWidget *parent = nullptr)
		: QPushButton(parent)
	{
		this->setChecked(true);
	}

protected:
	void paintEvent(QPaintEvent *event) {
	
	}


};

class ItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT

public:
	explicit ItemDelegate(QObject *parent = nullptr)
		: QAbstractItemDelegate(parent){};

    void paint(QPainter *painter,
			   const QStyleOptionViewItem &option,
			   const QModelIndex &index) const;

	QSize sizeHint(const QStyleOptionViewItem &option,
		   const QModelIndex &index) const;
};

class ComboBox : public QComboBox
{
	Q_OBJECT
public:
	explicit ComboBox(QWidget *parent = nullptr);
};

class SceneSettingsWidget : public QWidget
{
	Q_OBJECT
public:
	SceneSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}

private:
	ComboBox *combobox_scenes = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

	void initUi();
};

class IpCameraSettingsWidget : public QWidget {
	Q_OBJECT
public:
	IpCameraSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}

private:
	QComboBox *combobox_cameraName = nullptr;
	QLineEdit *lineedit_rtsp = nullptr;
	QComboBox *combobox_rate = nullptr;
	QComboBox *combobox_fps = nullptr;
	QComboBox *combobox_encode = nullptr;
	QComboBox *combobox_resolution = nullptr;

	void initUi();
};

class USBCameraSettingsWidget : public QWidget {
	Q_OBJECT
public:
	USBCameraSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}

private:
	QComboBox *combobox_cameraName = nullptr;
	QComboBox *combobox_rate = nullptr;
	QComboBox *combobox_fps = nullptr;
	QComboBox *combobox_encode = nullptr;
	QComboBox *combobox_resolution = nullptr;

	void initUi();
};



class AddScenesPanel : public QWidget
{
	Q_OBJECT

public:
	AddScenesPanel(QWidget *parent);
	~AddScenesPanel();

private:
	QLabel *pBtn_close = nullptr;
	QStackedWidget *stackedWidget = nullptr;
	QPushButton *tabBtn_1 = nullptr;
	QPushButton *tabBtn_2 = nullptr;
	QPushButton *tabBtn_3 = nullptr;
	QPushButton *pBtnCancel = nullptr;
	QPushButton *pBtnYes = nullptr;

	SceneSettingsWidget *sceneSettingsWidget = nullptr;
	IpCameraSettingsWidget *ipCameraSettingsWidget = nullptr;
	USBCameraSettingsWidget *usbCameraSettingsWidget = nullptr;

	bool mousePressed = false;
	QPoint cPos = QPoint();

protected:
	void paintEvent(QPaintEvent *event) override;
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	void initUi();

};

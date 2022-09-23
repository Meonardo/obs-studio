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
#include <qscrollbar.h>
#include <qframe.h>
#include <qpainterpath.h>
#include "font.hpp"

/*  Custom Combobox View */
class ComboBoxView : public QFrame
{
	Q_OBJECT
public:
	ComboBoxView(QWidget *parent = nullptr);
	void setRadius(int r) { this->radius = r; }
	void setShadow(int border) { shadowBorder = border; }
	inline int shadow() { return shadowBorder; }
	void setFixedSize(const QSize &size) { this->setFixedSize(size.width(), size.height()); }
	void setFixedSize(int w, int h) { QFrame::setFixedSize(this->shadowBorder * 2 + w, this->shadowBorder * 2 + h); }

protected:
	void paintEvent(QPaintEvent *event) override;
	bool event(QEvent *e) override;
private:
	int shadowBorder = 0;
	int radius = 0;
signals:
	void viewHide();
};

/*  Custom Combobox  */
class ComboBox : public QFrame {
	Q_OBJECT
public:
	explicit ComboBox(QWidget *parent = nullptr);

	void setText(const QString &text) {
		if (label_text != nullptr)
			label_text->setText(text);
	}

	bool isChecked() { return checked; }

protected:
	bool event(QEvent *e) override;

private:
	void initUi();

private:
	bool checked = false;
	QLabel *label_text = nullptr;
	QLabel *label_icon = nullptr;
	ComboBoxView *view = nullptr;
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

//class ComboBox : public QComboBox
//{
//	Q_OBJECT
//public:
//	explicit ComboBox(QWidget *parent = nullptr);
//};

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
	ComboBox *combobox_cameraName = nullptr;
	QLineEdit *lineedit_rtsp = nullptr;
	ComboBox *combobox_rate = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

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
	ComboBox *combobox_cameraName = nullptr;
	ComboBox *combobox_rate = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

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

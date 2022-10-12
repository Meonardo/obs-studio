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
#include <qlistwidget.h>
#include <qbuttongroup.h>
#include "font.hpp"
#include "obs-source-manager.h"
#include <qfontmetrics.h>
#include <qcheckbox.h>
#include <qtimer.h>

/************************************** ComboBox item widget ****************************/
class ComboBoxItemWidget : public QWidget {
	Q_OBJECT
public:
	explicit ComboBoxItemWidget(int index, int width, int height,
				    bool hideLine = false,
				    QWidget *parent = nullptr);
	void setText(const QString &text)
	{
		if (pBtn_text != nullptr)
			pBtn_text->setText(text);
	}
	inline QPushButton *getBtn() { return pBtn_text; }
	void setBtnEnabled(bool enabled) { pBtn_text->setEnabled(enabled); }

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	QPushButton *pBtn_text = nullptr;
	int itemIndex;
signals:
	void btnClicked(int, QString);
};

/************************************** Combobox View ****************************/
class ComboBoxView : public QWidget {
	Q_OBJECT
public:
	ComboBoxView(QWidget *parent = nullptr);
	inline int shadow() { return shadowBorder; }
	void addItems(QStringList list, QList<int> titleIndexList = QList<int>());
	void setMaxDisplayCount(int count);
	inline int getCurrentIndex() { return currentIndex; }
	void setFixedWidth(int w)
	{
		QWidget::setFixedWidth(w + 2 * shadowBorder);
	}

protected:
	void paintEvent(QPaintEvent *event) override;
	bool event(QEvent *e) override;

private:
	int itemCount = 0;
	int shadowBorder = 0;
	int maxDisplayCount = 1;
	int currentIndex = 0;
	int itemHeight = 0;
	QListWidget *listWidget = nullptr;
	QButtonGroup *itemGroup = nullptr;
	QMap<QListWidgetItem *, ComboBoxItemWidget *> itemMap;
signals:
	void viewHide();
	void itemIndexChanged(int, QString);
};

/************************************** Custom Combobox ****************************/
class ComboBox : public QFrame {
	Q_OBJECT
public:
	explicit ComboBox(QWidget *parent = nullptr);
	bool isChecked() { return checked; }
	void setMaxDisplayCount(int count) { view->setMaxDisplayCount(count); }
	void addItems(QStringList textList, QList<int> titleIndexList = QList<int>());
	inline QString getText() { return label_text->text(); }
	inline int currentIndex() { return m_Index; }

protected:
	bool event(QEvent *e) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	void initUi();
	void setText(const QString &text);

private:
	bool checked = false;
	QLabel *label_text = nullptr;
	QLabel *label_icon = nullptr;
	ComboBoxView *view = nullptr;
	int m_Index;
signals:
	void itemIndexChanged(QString);
};

/************************************** Scene settings widget ****************************/
class SceneSettingsWidget : public QWidget {
	Q_OBJECT
public:
	SceneSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}
	void initData(std::vector<
		      std::shared_ptr<accrecorder::source::ScreenSceneItem>>
			      screenItems);
	int getSceneIndex() { return combobox_scenes->currentIndex(); }

private:
	ComboBox *combobox_scenes = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

	void initUi();
};

/************************************** ipCamera settings widget ****************************/
class IpCameraSettingsWidget : public QWidget {
	Q_OBJECT
public:
	IpCameraSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}
	void initData();

	std::string getRTSPURL() { return lineedit_rtsp->text().toStdString(); }
	std::string getCameraName()
	{
		return combobox_cameraName->getText().toStdString();
	}
	void setEmptyStyle();

private:
	ComboBox *combobox_cameraName = nullptr;
	QLineEdit *lineedit_rtsp = nullptr;
	ComboBox *combobox_rate = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

	void initUi();
};

/************************************** usbCamera settings widget ****************************/
class USBCameraSettingsWidget : public QWidget {
	Q_OBJECT
public:
	USBCameraSettingsWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		this->initUi();
	}
	void initData(std::vector<
		      std::shared_ptr<accrecorder::source::CameraSceneItem>>
			      usbCameraSource);
	int getSceneIndex() { return combobox_cameraName->currentIndex(); }
	accrecorder::source::CameraSceneItem *getCurrentCameraSource();

	void
	updateDataSourceIfNeed(accrecorder::manager::OBSSourceManager *manager);

private:
	ComboBox *combobox_cameraName = nullptr;
	ComboBox *combobox_rate = nullptr;
	ComboBox *combobox_fps = nullptr;
	ComboBox *combobox_encode = nullptr;
	ComboBox *combobox_resolution = nullptr;

	std::vector<std::shared_ptr<accrecorder::source::CameraSceneItem>>
		usbCameraSource;
	QMap<QString, std::shared_ptr<accrecorder::source::CameraSceneItem>>
		mapCameraSource;

	void initUi();

private slots:
	void slot_combobox_cameraName_changed(QString);
};

/************************************** Basic panel ****************************/
class BasicPanel : public QWidget {
	Q_OBJECT
public:
	explicit BasicPanel(QWidget *parent = nullptr) : QWidget(parent)
	{
		shadowBorder = 4 * getScale();
		this->setWindowFlags(Qt::FramelessWindowHint);
		this->setAttribute(Qt::WA_TranslucentBackground, true);
	}
	~BasicPanel() {}

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
	int shadowBorder;
	bool mousePressed = false;
	QPoint cPos = QPoint();
};

/************************************** add scene panel ****************************/
class ScenesSettingsPanel : public BasicPanel {
	Q_OBJECT
public:
	ScenesSettingsPanel(accrecorder::manager::OBSSourceManager *manager,
			    accrecorder::source::SceneItem::Category categroy,
			    QWidget *parent = nullptr);
	~ScenesSettingsPanel() {}
	void show();

private:
	QPushButton *pBtn_close = nullptr;
	QStackedWidget *stackedWidget = nullptr;
	QPushButton *tabBtn_1 = nullptr;
	QPushButton *tabBtn_2 = nullptr;
	QPushButton *tabBtn_3 = nullptr;
	QPushButton *pBtnCancel = nullptr;
	QPushButton *pBtnYes = nullptr;
	QFrame *frame_top = nullptr;
	QTimer *m_timer = nullptr;

	SceneSettingsWidget *sceneSettingsWidget = nullptr;
	IpCameraSettingsWidget *ipCameraSettingsWidget = nullptr;
	USBCameraSettingsWidget *usbCameraSettingsWidget = nullptr;

	bool mousePressed = false;
	QPoint cPos = QPoint();

	accrecorder::manager::OBSSourceManager *sourceManager;
	accrecorder::source::SceneItem::Category itemCategory;
	std::vector<std::shared_ptr<accrecorder::source::ScreenSceneItem>> sceneItems;
	//std::vector<std::shared_ptr<accrecorder::source::CameraSceneItem>> cameraItems;

private:
	void initUi();
	void initData();

private slots:
	void slot_addBtn_clicked();

signals:
	void attachFinished(accrecorder::source::SceneItem *,
			    accrecorder::source::SceneItem::Category);
};

/************************************** audio panel ****************************/
class AudioSettingsPanel : public BasicPanel {
	Q_OBJECT
public:
	AudioSettingsPanel(accrecorder::manager::OBSSourceManager *manager,
			   QWidget *parent = nullptr);
	~AudioSettingsPanel() {}

private:
	ComboBox *combobox_audio = nullptr;
	accrecorder::manager::OBSSourceManager *sourceManager;

	void initUi();
	void initData();
};

/************************************** push stream panel ****************************/
class StreamingSettingsPanel : public BasicPanel {
	Q_OBJECT
public:
	StreamingSettingsPanel(accrecorder::manager::OBSSourceManager *manager,
			       QWidget *parent = nullptr);
	~StreamingSettingsPanel() {}

private:
	QLineEdit *lineedit_rtsp1 = nullptr;
	QLineEdit *lineedit_rtsp2 = nullptr;
	QLineEdit *lineedit_rtsp3 = nullptr;
	QCheckBox *checkbox_rtsp1 = nullptr;
	QCheckBox *checkbox_rtsp2 = nullptr;
	QCheckBox *checkbox_rtsp3 = nullptr;
	accrecorder::manager::OBSSourceManager *sourceManager;

	void initUi();
	void initData();
};

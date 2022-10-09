#pragma once

#include <QWidget>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qevent.h>
#include <qmap.h>
#include "horizontal-scroll-area.hpp"
#include "obs-source-manager.h"
#include "volume-control.hpp"

class OBSPanelItem : public QWidget {
	Q_OBJECT
public:
	OBSPanelItem(int type, const QString &name, int index,
		     QWidget *parent = nullptr);
	void setChecked(bool checked) { checkbox->setChecked(checked); }
	inline QAbstractButton *getCheckButton() { return checkbox; }
	void setIndex(int index) { m_index = index; }

private:
	int m_type;
	int m_index;
	QString m_name;

	QFrame *frame = nullptr;
	QLabel *label_icon = nullptr;
	QLabel *label_name = nullptr;
	QCheckBox *checkbox = nullptr;

	void initUi();
	void setIcon(bool checked);

protected:
	void paintEvent(QPaintEvent *event);
	bool event(QEvent *e);

signals:
	void itemCheckedStateChanged(int, bool);
	void deleteItem(int);
};

class OBSPanel : public QWidget {
	Q_OBJECT
public:
	OBSPanel(const QString &title, QWidget *parent);
	~OBSPanel() {}
	void addItem(accrecorder::source::SceneItem *item);
	void addAudioItem(VolControl *item);
	void removeAudioItem(VolControl *item);
	void setManager(accrecorder::manager::OBSSourceManager *manager);
	void setFirstItemChecked();

private:
	QString m_title;
	QFrame *listFrame;
	QWidget *scrollAreaContentWidget;
	QVBoxLayout *contentWidgetLayout;
	HScrollArea *hscrollArea;
	int itemCount = 0;
	QButtonGroup *checkBoxGroup;

	QList<accrecorder::source::SceneItem *> sceneItemList;
	QList<OBSPanelItem *> panelItemList;
	QList<VolControl *> volItemList;
	accrecorder::manager::OBSSourceManager *sourceManager;

	void initUi();

protected:
	void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *obj, QEvent *event) override;

signals:
	void addClicked();
};

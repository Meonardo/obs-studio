#pragma once

#include <QWidget>
#include <qlabel.h>

class OBSPanel  : public QWidget
{
	Q_OBJECT

public:
	OBSPanel(const QString &title, QWidget *parent);
	~OBSPanel();

private:
	QLabel *pBtn_add = nullptr;
	QString m_title;

	void initUi();

protected:
	void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *obj, QEvent *event) override;

signals:
	void addNewScene();

};

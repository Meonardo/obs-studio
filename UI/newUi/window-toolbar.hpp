#pragma once

#include <QWidget>
#include <qpushbutton.h>
#include <qlabel.h>

class OBSToolbar : public QWidget {
	Q_OBJECT

public:
	OBSToolbar(QWidget *parent);
	~OBSToolbar();

protected:
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	void initUi();

	QPushButton *pBtn_settings_stream;
	QLabel *label_settings_stream;

signals:
	void showStreamPanel();
};

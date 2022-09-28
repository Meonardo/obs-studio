#pragma once

#include <QWidget>

class OBSToolbar  : public QWidget
{
	Q_OBJECT

public:
	OBSToolbar(QWidget *parent);
	~OBSToolbar();

protected:
	void paintEvent(QPaintEvent *event);

private:
	void initUi();

signals:
	void showStreamPanel();
};

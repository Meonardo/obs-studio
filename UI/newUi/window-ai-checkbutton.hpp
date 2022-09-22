#pragma once

#include <qwidget.h>
#include <qframe.h>

class OBSAiCheckButton : public QWidget
{
	Q_OBJECT

public:
	OBSAiCheckButton(QWidget *parent);
	~OBSAiCheckButton();

protected:
	void paintEvent(QPaintEvent *event);

private:
	QFrame *m_frame = nullptr;


private:
	void initUi();

};

#pragma once

#include <QWidget>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qevent.h>
#include <qmainwindow.h>
#include <qscreen.h>

enum class DragDirection
{
	DragVer,
	DragHorLeft,
	DragHorRight,
	DragBDiag,
	DragFDiag
};

class TitleBar : public QWidget {
	Q_OBJECT
public:
	TitleBar(const QString &title, QMainWindow *window);

protected:
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	QMainWindow *mainWindow = nullptr;
	bool mousePressed = false;

	bool dragSize = false;
	DragDirection dragDirection;

	QPoint pressedPos;
	qreal x_scale;
	int borderWidth;	// Use to drag border to change window size

private slots:
	void pBtn_fullScreen_clicked_slot();
};

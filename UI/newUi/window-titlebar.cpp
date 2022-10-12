#include "window-titlebar.hpp"
#include "window-basic-main.hpp"

#include "font.hpp"

TitleBar::TitleBar(const QString &title, QMainWindow *window)
	:  mainWindow(window)
{
	QLabel *label_title = new QLabel(this);
	label_title->setText(title);
	label_title->setStyleSheet(QString("QLabel{color: rgb(34, 34, 34); %1}")
					   .arg(getFontStyle(14)));
	label_title->setMouseTracking(true);

	auto pBtn_fullScreen = new QPushButton(this);
	pBtn_fullScreen->setFixedSize(20 * getScale(), 20 * getScale());
	pBtn_fullScreen->setStyleSheet(
		QString("QPushButton{background-color: transparent; "
			"border: none; padding: 0px; min-width: %1px; min-height: %2px;"
			"border-image: url(':/res/images/newUi/fullscreen@2x.png'); }")
			.arg(pBtn_fullScreen->width())
			.arg(pBtn_fullScreen->height()));
	pBtn_fullScreen->installEventFilter(this);
	pBtn_fullScreen->setMouseTracking(true);
	connect(pBtn_fullScreen, &QPushButton::clicked, this, &TitleBar::pBtn_fullScreen_clicked_slot);

	auto pBtn_close = new QPushButton(this);
	pBtn_close->setFixedSize(20 * getScale(), 20 * getScale());
	pBtn_close->setStyleSheet(
		QString("QPushButton{background-color: transparent; "
			"border: none; padding: 0px; min-width: %1px; min-height: %2px;"
			"border-image: url(':/res/images/newUi/close@2x.png');}")
			.arg(pBtn_fullScreen->width())
			.arg(pBtn_fullScreen->height()));
	pBtn_close->installEventFilter(this);
	pBtn_close->setMouseTracking(true);
	connect(pBtn_close, &QPushButton::clicked, this,
		[=]() {
			#if DEBUG
			mainWindow->showMinimized();
			#else
			auto window = reinterpret_cast<OBSBasic *>(mainWindow);
			if (window != nullptr)
				window->ToggleMainWindowHide(true);
			#endif
		});

	QHBoxLayout *layout_title = new QHBoxLayout(this);
	layout_title->setContentsMargins(20 * getScale(), 0,
			15 * getScale(), 0);
	layout_title->setSpacing(10 * getScale());
	layout_title->addWidget(label_title, 1, Qt::AlignVCenter);
	layout_title->addWidget(pBtn_fullScreen, 0, Qt::AlignVCenter);
	layout_title->addWidget(pBtn_close, 0, Qt::AlignVCenter);

	this->setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setBrush(QPalette::Background, QBrush(QColor(255, 255, 255)));
	this->setPalette(palette);

	this->installEventFilter(this);
	this->setMouseTracking(true);

	borderWidth = 5 * getScale();	
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		this->pBtn_fullScreen_clicked_slot();
	}
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == this && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			mousePressed = true;
			pressedPos = mouseevent->pos();
			if (mainWindow->screen()->geometry() ==
			    mainWindow->geometry()) {
				x_scale = (qreal)pressedPos.x() / mainWindow->width();
			}

			dragSize = true;
			if (this->cursor() == QCursor(Qt::SizeFDiagCursor))
				dragDirection = DragDirection::DragFDiag;
			else if (this->cursor() == QCursor(Qt::SizeBDiagCursor))
				dragDirection = DragDirection::DragBDiag;
			else if (this->cursor() == QCursor(Qt::SizeVerCursor))
				dragDirection = DragDirection::DragVer;
			else if (this->cursor() == QCursor(Qt::SizeHorCursor)) {
				if (pressedPos.x() <= borderWidth)
					dragDirection = DragDirection::DragHorLeft;
				else
					dragDirection = DragDirection::DragHorRight;
			}else
				dragSize = false;
		}
	} else if (obj == this && event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (mouseevent->button() == Qt::LeftButton) {
			if (mousePressed && !dragSize && mouseevent->globalPos().y() <= 5 &&
			    mainWindow->screen()->geometry() !=
				    mainWindow->geometry()) {
				pBtn_fullScreen_clicked_slot();
			}
			mousePressed = false;
		}
	} else if (obj == this && event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseevent = static_cast<QMouseEvent *>(event);
		if (!mousePressed) {
			QPoint pos = mouseevent->pos();
			if (pos.x() <= borderWidth && pos.y() <= borderWidth) {
				this->setCursor(QCursor(Qt::SizeFDiagCursor));
			} else if (pos.x() >= (this->width() - borderWidth) &&
				   pos.y() <= borderWidth) { 
				this->setCursor(QCursor(Qt::SizeBDiagCursor));
			} else if (pos.y() < borderWidth) {
				this->setCursor(QCursor(Qt::SizeVerCursor));
			} else if (pos.x() < borderWidth ||
				   pos.x() >= (this->width() - borderWidth)) {
				this->setCursor(Qt::SizeHorCursor);
			} else {
				this->setCursor(Qt::ArrowCursor);
			}
			return true;
		}

		int x_offset, y_offset;
		QPoint globalPos = mouseevent->globalPos();
		if (dragSize) {
			QRect rect = mainWindow->geometry();
			if (dragDirection == DragDirection::DragHorLeft) {
				rect.setLeft(globalPos.x() - pressedPos.x());
			} else if (dragDirection == DragDirection::DragHorRight) {
				//rect.setRight(globalPos.x() + mainWindow->width() - pressedPos.x());
				rect.setRight(globalPos.x());
			} else if (dragDirection == DragDirection::DragVer) {
				rect.setTop(globalPos.y());
			} else if (dragDirection == DragDirection::DragFDiag) {
				rect.setTopLeft(QPoint(globalPos.x() - pressedPos.x(), globalPos.y() - pressedPos.y()));
			} else if (dragDirection == DragDirection::DragBDiag) {
				rect.setTopRight(QPoint(globalPos.x() + mainWindow->width() - pressedPos.x(),
					globalPos.y() - pressedPos.y()));
			}
			if (rect.width() >= mainWindow->minimumWidth() &&
			    rect.height() >= mainWindow->minimumHeight()) {
				if (dragDirection == DragDirection::DragHorRight ||
				    dragDirection == DragDirection::DragBDiag) {
					pressedPos.setX(pressedPos.x() + rect.right() - mainWindow->geometry().right());
				}
				mainWindow->setGeometry(rect);
			}
			return true;
		}
		else
		{
			y_offset = globalPos.y() - pressedPos.y();
			QScreen *screen = mainWindow->screen();
			if (screen->geometry() == mainWindow->geometry()) {
				int x, y, w, h;
				if (mainWindow->property("normal_width")
					    .toInt() <= 0) {
					QRect rect =
						mainWindow->normalGeometry();
					x = rect.x();
					y = rect.y();
					w = rect.width();
					h = rect.height();
				} else {
					x = mainWindow->property("normal_x")
						    .toInt();
					y = mainWindow->property("normal_y")
						    .toInt();
					w = mainWindow->property("normal_width")
						    .toInt();
					h = mainWindow
						    ->property("normal_height")
						    .toInt();
				}
				pressedPos.setX(w * x_scale);
				mainWindow->resize(w, h);
			}

			x_offset = globalPos.x() - pressedPos.x();
			mainWindow->move(x_offset, y_offset);
		}
	}else if (obj != this && event->type() == QEvent::MouseMove) {
		this->setCursor(Qt::ArrowCursor);
		return true;
	} 
	return QWidget::eventFilter(obj, event);
}

void TitleBar::pBtn_fullScreen_clicked_slot()
{
	QScreen *screen = mainWindow->screen();
	if (screen->geometry() == mainWindow->geometry()) {
		if (mainWindow->property("normal_width").toInt() <= 0) {
			QRect normalRect = mainWindow->normalGeometry();
			mainWindow->setProperty("normal_width",
						normalRect.width());
			mainWindow->setProperty("normal_height",
						normalRect.height());
			mainWindow->setProperty("normal_x", normalRect.x());
			mainWindow->setProperty("normal_y", normalRect.y());
		}
		QRect rect =
			QRect(mainWindow->property("normal_x").toInt(),
			      mainWindow->property("normal_y").toInt(),
			      mainWindow->property("normal_width").toInt(),
			      mainWindow->property("normal_height").toInt());
		mainWindow->setGeometry(rect);
	} else {
		mainWindow->setProperty("normal_width", mainWindow->width());
		mainWindow->setProperty("normal_height", mainWindow->height());
		mainWindow->setProperty("normal_x", mainWindow->x());
		mainWindow->setProperty("normal_y", mainWindow->y());

		mainWindow->setGeometry(screen->geometry());
	}
}


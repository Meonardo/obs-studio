#include "m-scene-tree.hpp"

#include <QSizePolicy>
#include <QScrollBar>
#include <QDropEvent>
#include <QPushButton>
#include <QTimer>

MSceneTree::MSceneTree(QWidget *parent_) : QListWidget(parent_)
{
	installEventFilter(this);
	setDragDropMode(InternalMove);
	setMovement(QListView::Snap);
}

#pragma once

#include <QListWidget>


class MSceneTree : public QListWidget
{
	Q_OBJECT
public:
	explicit MSceneTree(QWidget *parent = nullptr);
};

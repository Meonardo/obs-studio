#include "window-panel-main.hpp"

OBSPanelMainScreen::OBSPanelMainScreen(QWidget *parent)
	: OBSPanel(parent)
{
	m_title = QString(tr("Main screen"));
	this->initUi();
}

OBSPanelMainScreen::~OBSPanelMainScreen() {}


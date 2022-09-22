#include "window-panel-subscreen.hpp"

OBSPanelSubScreen::OBSPanelSubScreen(QWidget *parent)
	: OBSPanel(parent)
{
	m_title = QString(tr("Sub screen"));
	this->initUi();
}

OBSPanelSubScreen::~OBSPanelSubScreen() {}

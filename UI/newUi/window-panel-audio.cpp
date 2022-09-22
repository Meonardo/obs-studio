#include "window-panel-audio.hpp"

OBSPanelAudio::OBSPanelAudio(QWidget *parent)
	: OBSPanel(parent)
{
	m_title = QString(tr("音轨"));
	this->initUi();
}

OBSPanelAudio::~OBSPanelAudio() {}

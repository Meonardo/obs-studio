#pragma once

#include "obs-sources.h"

namespace accrecorder {
namespace manager {

class OBSSourceManager {
public:
	OBSSourceManager();
	~OBSSourceManager();

	// rename the scene item
	bool Rename(source::SceneItem *item, std::string& newName);
	// attach the scene item to the main scene
	bool AttachSceneItem(source::SceneItem* item);
	// apply scene item properties settings(internal settings)
	bool ApplySceneItemPropertiesUpdate(source::SceneItem *item);
	// apply scene item settings update(in the scene)
	bool ApplySceneItemSettingsUpdate(source::SceneItem *item);

	// list all the screens
	void ListScreenItems(
		std::vector<std::shared_ptr<source::ScreenSceneItem>> &items);
	// create a IPCamera scene item
	std::shared_ptr<source::IPCameraSceneItem>
	CreateIPCameraItem(std::string &name, std::string &url);

private:
	void RemoveScene(std::string& name);
	obs_scene_t* CreateScene(std::string& name);

	static obs_source_t *ValidateScene(std::string &name);
	static obs_source_t *ValidateInput(std::string &name);

	source::Scene* main_scene_;
};

}; //namespace manager
}; //namespace accrecorder

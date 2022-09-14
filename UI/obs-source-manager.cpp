#include "obs-source-manager.h"

namespace accrecorder {	
namespace manager {

OBSSourceManager::OBSSourceManager() : main_scene_(nullptr)
{
	std::string sceneName(kMainScene);
	// remove old scene first.
	RemoveScene(sceneName);
	// then create new one.
	obs_scene_t* scene = CreateScene(sceneName);
	if (scene == nullptr) {
		blog(LOG_ERROR, "can not create scene(%s)\n", sceneName.c_str());
	}
	// save it.
	main_scene_ = new source::Scene(sceneName, scene);
}

OBSSourceManager::~OBSSourceManager()
{
	if (main_scene_ != nullptr) {
		delete main_scene_;
		main_scene_ = nullptr;
	}
}

void OBSSourceManager::RemoveScene(std::string& name)
{
	OBSSourceAutoRelease scene = ValidateScene(name);
	if (!scene) {
		blog(LOG_ERROR, "remove scene(%s) failed!\n", name.c_str());
	}
	obs_source_remove(scene);
	blog(LOG_INFO, "scene(%s) removed!", name.c_str());
}

obs_scene_t* OBSSourceManager::CreateScene(std::string& sceneName)
{
	if (sceneName.empty())
		return nullptr;
	OBSSourceAutoRelease scene = obs_get_source_by_name(sceneName.c_str());
	if (scene) {
		blog(LOG_ERROR, "the scene with name(%s) already exist!\n",
		     sceneName.c_str());
		return nullptr;
	}
	obs_scene_t* createdScene = obs_scene_create(sceneName.c_str());
	if (!createdScene) {
		blog(LOG_ERROR, "create scene(%s) failed!\n", sceneName.c_str());
		return nullptr;
	}

	return createdScene;
}

obs_source_t* OBSSourceManager::ValidateScene(std::string& name)
{
	obs_source_t *ret = obs_get_source_by_name(name.c_str());
	if (!ret)
		return nullptr;

	if (obs_source_get_type(ret) != OBS_SOURCE_TYPE_SCENE) {
		obs_source_release(ret);
		blog(LOG_ERROR, "(%s) is not a valid scene source!\n", name.c_str());
		return nullptr;
	}
	return ret;
}

obs_source_t *OBSSourceManager::ValidateInput(std::string &name)
{
	obs_source_t *ret = obs_get_source_by_name(name.c_str());
	if (!ret) {
		blog(LOG_ERROR, "the source with name(%s) not exist!\n",
		     name.c_str());
		return nullptr;
	}

	if (obs_source_get_type(ret) != OBS_SOURCE_TYPE_INPUT) {
		obs_source_release(ret);
		blog(LOG_ERROR, "the source with name(%s) is not an input!\n",
		     name.c_str());
		return nullptr;
	}

	return ret;
}

bool OBSSourceManager::AttachSceneItem(source::SceneItem *item)
{
	if (main_scene_ == nullptr) {
		blog(LOG_ERROR, "scene source not exist!");
		return false;
	}

	main_scene_->Attach(item);
	return true;
}

bool OBSSourceManager::ApplySceneItemPropertiesUpdate(source::SceneItem *item)
{
	if (main_scene_ == nullptr) {
		blog(LOG_ERROR, "scene source not exist!");
		return false;
	}
	OBSSourceAutoRelease src = obs_get_source_by_name(item->Name().c_str());
	if (src != nullptr) {
		blog(LOG_ERROR,
		     "can not update settings without attach the item to the scene!");
		return false;
	}
	OBSDataAutoRelease inputSettings = item->Properties();
	// apply settings
	obs_source_reset_settings(src, inputSettings);
	// refresh UI element
	obs_source_update_properties(src);

	return true;
}

bool OBSSourceManager::Rename(source::SceneItem *item, std::string &newName)
{
	if (newName.empty() || item->Name() == newName) {
		blog(LOG_INFO, "invalid source name or no need update!");
		return false;
	}

	OBSSourceAutoRelease input = ValidateInput(item->Name());
	if (input == nullptr) {
		blog(LOG_INFO, "no source found!");
		return false;
	}
	OBSSourceAutoRelease existingSource = obs_get_source_by_name(newName.c_str());
	if (existingSource != nullptr) {
		blog(LOG_INFO, "the new name(%s) source already exist!",
		     newName.c_str());
		return false;
	}

	// finally, here we update the source name
	obs_source_set_name(input, newName.c_str());
	// do not forget update the SceneItem
	item->SetName(newName);

	return true;
}

bool OBSSourceManager::ApplySceneItemSettingsUpdate(source::SceneItem *item)
{
	if (main_scene_ == nullptr) {
		blog(LOG_ERROR, "scene source not exist!");
		return false;
	}

	return main_scene_->ApplySceneItemSettingsUpdate(item);
}

void OBSSourceManager::ListScreenItems(
	std::vector<std::shared_ptr<source::ScreenSceneItem>> &items)
{
	const char *tmpName = "tmp";
	const char *prop_name = "monitor";
	const char *kind = "monitor_capture";
	OBSSourceAutoRelease source = obs_get_source_by_name(tmpName);
	if (source != nullptr) {
		blog(LOG_ERROR, "can not enum screen list.");
		return;
	}

	// create tmp source
	source = obs_source_create(kind, tmpName, NULL, nullptr);

	// get properties
	obs_properties_t *props = obs_source_properties(source);
	OBSDataAutoRelease settings = obs_source_get_settings(source);
	std::string cur_id =
		std::to_string(obs_data_get_int(settings, prop_name));

	// get detail
	obs_property_t *p = obs_properties_get(props, prop_name);
	size_t count = obs_property_list_item_count(p);

	for (size_t i = 0; i < count; i++) {
		const char *name = obs_property_list_item_name(p, i);
		int id = (int)obs_property_list_item_int(p, i);
		blog(LOG_ERROR, "Enum monitor: %s, id=%d", name, id);
		auto item =
			std::make_shared<source::ScreenSceneItem>(
				std::string(name));
		item->index = id;
		items.push_back(item);
	}
}

std::shared_ptr<source::IPCameraSceneItem>
OBSSourceManager::CreateIPCameraItem(std::string &name,
							      std::string &url)
{
	return std::make_shared<source::IPCameraSceneItem>(name, url, false);
}

} //namespace manager 
} //namespace accrecorder

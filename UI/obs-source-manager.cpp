#include "obs-source-manager.h"

#include <chrono>
#include <thread>

static void SplitString(std::string &source, std::string &&token,
			std::vector<std::string> &result)
{
	size_t start = 0;
	size_t end = source.find(token);
	while (end != std::string::npos) {
		result.emplace_back(source.substr(start, end - start));
		start = end + token.length();
		end = source.find(token, start);
	}
}

namespace accrecorder::manager {
OBSSourceManager::OBSSourceManager() : main_scene_(nullptr)
{
	std::string sceneName(kMainScene);
	obs_source_t *scene = ValidateScene(sceneName);
	if (scene != nullptr) {
		// save it.
		main_scene_ = new source::Scene(sceneName,
						obs_scene_from_source(scene));
		// load scene from the scene
		LoadSceneItemFromScene(sceneName);
	} else {
		// create new one.
		obs_scene_t *newScene = CreateScene(sceneName);
		if (newScene == nullptr) {
			blog(LOG_ERROR, "can not create scene(%s)\n",
			     sceneName.c_str());
		}
		// save it.
		main_scene_ = new source::Scene(sceneName, newScene);
	}

	// make current
	OBSSourceAutoRelease mainScene = ValidateScene(sceneName);
	if (mainScene != nullptr) {
		obs_frontend_set_current_scene(mainScene);
	}
}

OBSSourceManager::~OBSSourceManager()
{
	if (main_scene_ != nullptr) {
		delete main_scene_;
		main_scene_ = nullptr;
	}
}

bool OBSSourceManager::IsMainSceneCreated() const
{
	return main_scene_ != nullptr && main_scene_->items_.size() > 0;
}

void OBSSourceManager::AvailableSceneItems(
	std::vector<source::SceneItem *> &items) const
{
	for (auto &item : main_scene_->items_) {
		items.push_back(item);
	}
}

void OBSSourceManager::LoadSceneItemFromScene(std::string &sceneName)
{
	OBSSourceAutoRelease scene = ValidateScene(sceneName);
	if (!scene) {
		blog(LOG_ERROR, "remove scene(%s) failed!\n",
		     sceneName.c_str());
	}

	blog(LOG_INFO, "enum scene item in the scene");
	auto cb = [](obs_scene_t *, obs_sceneitem_t *sceneItem, void *param) {
		auto enumData =
			static_cast<std::vector<source::SceneItem *> *>(param);
		// get scene item
		OBSSource itemSource = obs_sceneitem_get_source(sceneItem);
		// get settings
		obs_data_t *settings = obs_source_get_settings(itemSource);
		// get name
		std::string name(obs_source_get_name(itemSource));
		// get id
		uint64_t sceneItemId = obs_sceneitem_get_id(sceneItem);

		if (obs_source_get_type(itemSource) == OBS_SOURCE_TYPE_INPUT) {
			const char *sourceType = obs_source_get_id(itemSource);
			std::string inputType(sourceType);

			if (inputType == "monitor_capture") {
				// screen scene item
				auto item = new source::ScreenSceneItem(name);
				item->SetSceneID(sceneItemId);
				enumData->push_back(item);
				item->type_ = source::SceneItem::Type::kScreen;
				// get settings
				GetSettingValueWithName<int>(
					settings, "monitor", item->index);
				GetSettingValueWithName<int>(
					settings, "method",
					item->capture_method);
				GetSettingValueWithName<bool>(
					settings, "cursor", item->show_cursor);

			} else if (inputType == "gstreamer-source") {
				// gst source(IPCamera)
				std::string pipeline = "";
				auto item = new source::IPCameraSceneItem(
					name, pipeline, false);
				item->type_ =
					source::SceneItem::Type::kIPCamera;
				item->SetSceneID(sceneItemId);
				enumData->push_back(item);
				// get settings
				GetSettingValueWithName(settings, "pipeline",
							pipeline);
				auto results = std::vector<std::string>();
				SplitString(pipeline, "=", results);
				if (results.size() > 1) {
					auto tmp = results[1];
					auto uri =
						tmp.substr(0, tmp.length() - 5);
					item->UpdateURL(uri);
				}
				GetSettingValueWithName<bool>(
					settings, "stop_on_hide",
					item->stop_on_hide_);

			} else if (inputType == "dshow_input") {
				// usb cameras scene item
				auto item = new source::CameraSceneItem(name);
				item->SetSceneID(sceneItemId);
				enumData->push_back(item);
				item->type_ = source::SceneItem::Type::kCamera;
				// get settings
				GetSettingValueWithName(settings,
							"video_device_id",
							item->device_id_);
				GetSettingValueWithName(settings, "resolution",
							item->selected_res_);
				/*int fps = 0;
				GetSettingValueWithName<int>(
					settings, "frame_interval", fps);*/

			} else if (inputType == "wasapi_output_capture") {
				// speakers
				auto item = new source::AudioOutputItem(name);
				item->SetSceneID(sceneItemId);
				enumData->push_back(item);
				item->type_ =
					source::SceneItem::Type::kAudioOutput;
				// get settings
				GetSettingValueWithName(settings, "device_id",
							item->device_id_);
			} else if (inputType == "wasapi_input_capture") {
				// microphones
				auto item = new source::AudioInputItem(name);
				item->SetSceneID(sceneItemId);
				enumData->push_back(item);
				item->type_ =
					source::SceneItem::Type::kAudioInput;
				// get settings
				GetSettingValueWithName(settings, "device_id",
							item->device_id_);
			}
		}

		return true;
	};

	// enum scene item and save it to the scene object.
	obs_scene_enum_items(obs_scene_from_source(scene), cb,
			     &main_scene_->items_);

	// for now tmp enum all the cameras & filter the target camera then restore the settings.
	std::vector<std::shared_ptr<source::CameraSceneItem>> cameras;
	ListCameraItems(cameras);
	for (auto &item : main_scene_->items_) {
		if (item->type() == source::SceneItem::Type::kCamera) {
			auto target =
				reinterpret_cast<source::CameraSceneItem *>(
					item);
			for (auto &camera : cameras) {
				if (target->device_id_ == camera->device_id_) {
					target->fps_ = camera->fps_;
					target->resolutions_ =
						camera->resolutions_;
					break;
				}
			}
		}
	}

	// clean all the cameras that tmp created.
	cameras.clear();

	blog(LOG_INFO, "done with enum all the scene item in the scene");
}

void OBSSourceManager::RemoveScene(std::string &name)
{
	OBSSourceAutoRelease scene = ValidateScene(name);
	if (!scene) {
		blog(LOG_ERROR, "remove scene(%s) failed!\n", name.c_str());
	}

	blog(LOG_INFO, "enum scene item in the scene");
	std::vector<std::string> sceneItemNames;
	auto cb = [](obs_scene_t *, obs_sceneitem_t *sceneItem, void *param) {
		auto enumData = static_cast<std::vector<std::string> *>(param);
		OBSSource itemSource = obs_sceneitem_get_source(sceneItem);
		std::string name(obs_source_get_name(itemSource));
		enumData->push_back(name);
		return true;
	};
	obs_scene_enum_items(obs_scene_from_source(scene), cb, &sceneItemNames);

	// remove all the item in the scene
	for (auto &name : sceneItemNames) {
		blog(LOG_INFO, "remove scene item name: %s", name.c_str());
		OBSSourceAutoRelease input = ValidateInput(name);
		obs_source_remove(input);
	}

	obs_source_remove(scene);
	blog(LOG_INFO, "scene(%s) removed!", name.c_str());
}

obs_scene_t *OBSSourceManager::CreateScene(std::string &sceneName)
{
	if (sceneName.empty())
		return nullptr;
	OBSSourceAutoRelease scene = obs_get_source_by_name(sceneName.c_str());
	if (scene) {
		blog(LOG_ERROR, "the scene with name(%s) already exist!\n",
		     sceneName.c_str());
		return nullptr;
	}
	obs_scene_t *createdScene = obs_scene_create(sceneName.c_str());
	if (!createdScene) {
		blog(LOG_ERROR, "create scene(%s) failed!\n",
		     sceneName.c_str());
		return nullptr;
	}

	return createdScene;
}

obs_source_t *OBSSourceManager::ValidateScene(std::string &name)
{
	obs_source_t *ret = obs_get_source_by_name(name.c_str());
	if (!ret)
		return nullptr;

	if (obs_source_get_type(ret) != OBS_SOURCE_TYPE_SCENE) {
		obs_source_release(ret);
		blog(LOG_ERROR, "(%s) is not a valid scene source!\n",
		     name.c_str());
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
	if (src == nullptr) {
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
	OBSSourceAutoRelease existingSource =
		obs_get_source_by_name(newName.c_str());
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
	// get detail
	obs_property_t *p = obs_properties_get(props, prop_name);
	size_t count = obs_property_list_item_count(p);

	for (size_t i = 0; i < count; i++) {
		const char *name = obs_property_list_item_name(p, i);
		int id = (int)obs_property_list_item_int(p, i);
		blog(LOG_ERROR, "enum monitor: %s, id=%d", name, id);
		auto item = std::make_shared<source::ScreenSceneItem>(
			std::string(name));
		item->index = id;
		items.push_back(item);
	}

	// release properties for enum device list.
	obs_properties_destroy(props);
}

source::IPCameraSceneItem *
OBSSourceManager::CreateIPCameraItem(std::string &name, std::string &url)
{
	return new source::IPCameraSceneItem(name, url, false);
}

void OBSSourceManager::ListCameraItems(
	std::vector<std::shared_ptr<source::CameraSceneItem>> &items)
{
	const char *tmpName = "tmpUSBCam";
	const char *kind = "dshow_input";
	const char *prop_name = "video_device_id";
	const char *resolution_p_name = "resolution";
	const char *fps_p_name = "frame_interval";
	OBSSourceAutoRelease source = obs_get_source_by_name(tmpName);
	if (source != nullptr) {
		blog(LOG_ERROR, "can not enum camera list.");
		return;
	}
	// create tmp source
	source = obs_source_create(kind, tmpName, NULL, nullptr);

	// get properties
	obs_properties_t *props = obs_source_properties(source);
	// get detail
	obs_property_t *p = obs_properties_get(props, prop_name);
	size_t count = obs_property_list_item_count(p);

	for (size_t i = 0; i < count; i++) {
		const char *name = obs_property_list_item_name(p, i);
		const char *id = obs_property_list_item_string(p, i);
		blog(LOG_ERROR, "enum device id: %s, id=%s", name, id);

		auto item = std::make_shared<source::CameraSceneItem>(
			std::string(name));
		item->device_id_ = std::string(id);

		// make a fake selection in order to get resolution properties
		obs_data_t *data = obs_data_create();
		obs_data_set_string(data, prop_name, id);
		obs_source_reset_settings(source, data);
		obs_source_update_properties(source);
		// get properties again
		obs_properties_t *props1 = obs_source_properties(source);
		obs_property_t *p_res =
			obs_properties_get(props1, resolution_p_name);
		size_t count_res = obs_property_list_item_count(p_res);

		item->resolutions_.reserve(count_res);
		for (size_t j = 0; j < count_res; j++) {
			const char *res = obs_property_list_item_name(p_res, j);
			blog(LOG_ERROR, "enum device(%s), resolution=%s", name,
			     res);
			item->resolutions_.emplace_back(res);
		}

		if (count_res > 0) {
			// set default resolution value
			item->SelectResolution(0);

			// make a fake resolution selection
			obs_data_set_string(data, resolution_p_name,
					    item->selected_res_.c_str());
			obs_data_set_int(data, "res_type", 1);
			obs_source_reset_settings(source, data);
			obs_source_update_properties(source);
			// get properties again
			obs_properties_t *props2 =
				obs_source_properties(source);
			// get all fps
			obs_property_t *p_fps =
				obs_properties_get(props2, fps_p_name);
			size_t count_fps = obs_property_list_item_count(p_fps);

			item->fps_.reserve(count_fps);
			for (size_t k = 0; k < count_fps; k++) {
				const char *fps_desc =
					obs_property_list_item_name(p_fps, k);
				int64_t fps =
					obs_property_list_item_int(p_fps, k);
				blog(LOG_ERROR,
				     "enum device(%s), fps desc=%s, value=%d",
				     name, fps_desc, fps);
				item->fps_.emplace_back(std::make_tuple(
					std::string(fps_desc), fps));
			}

			// set default fps value(match the output of the obs)
			item->SelectFps(0);
		}

		// release anything just created for tmp using.
		obs_properties_destroy(props1);
		obs_data_release(data);

		items.push_back(item);
	}

	// release properties for enum device list.
	obs_properties_destroy(props);
}

void OBSSourceManager::ListAudioItems(
	std::vector<std::shared_ptr<source::AudioSceneItem>> &items, bool input)
{
	const char *tmpName = "tmpAudio";
	const char *prop_name = "device_id";
	char *kind = "wasapi_input_capture";
	if (input) {
		kind = "wasapi_output_capture";
	}

	OBSSourceAutoRelease source = obs_get_source_by_name(tmpName);
	if (source != nullptr) {
		blog(LOG_ERROR, "can not enum audio item list.");
		return;
	}

	// create tmp source
	source = obs_source_create(kind, tmpName, NULL, nullptr);

	// get properties
	obs_properties_t *props = obs_source_properties(source);
	// get detail
	obs_property_t *p = obs_properties_get(props, prop_name);
	size_t count = obs_property_list_item_count(p);

	for (size_t i = 0; i < count; i++) {
		const char *name = obs_property_list_item_name(p, i);
		const char *id = obs_property_list_item_string(p, i);
		blog(LOG_ERROR, "enum audio device(%s): %s, id=%s",
		     input ? "input" : "output", name, id);

		std::string s_name(name);
		// do not show default device for now
		if (s_name == "default")
			continue;
		if (input) {
			auto item = std::make_shared<source::AudioInputItem>(
				std::string(name));
			item->device_id_ = id;
			items.push_back(item);
		} else {
			auto item = std::make_shared<source::AudioOutputItem>(
				std::string(name));
			item->device_id_ = id;
			items.push_back(item);
		}
	}

	// release properties for enum device list.
	obs_properties_destroy(props);
}

source::SceneItem *OBSSourceManager::GetSceneItemByName(std::string &name)
{
	if (!IsMainSceneCreated())
		return nullptr;
	for (const auto &item : main_scene_->items_) {
		if (item->Name() == name) {
			return item;
		}
	}
	return nullptr;
}

bool OBSSourceManager::RemoveSceneItemByName(std::string &name)
{
	auto item = GetSceneItemByName(name);
	if (item == nullptr)
		return false;

	return Remove(item);
}

bool OBSSourceManager::Remove(source::SceneItem *item)
{
	if (item == nullptr)
		return false;

	// remove from obs source tree
	OBSSourceAutoRelease input = ValidateInput(item->Name());
	obs_source_remove(input);

	// detach from Scene
	return main_scene_->Detach(item);
}

template<typename T>
void OBSSourceManager::GetSettingValueWithName(obs_data_t *d,
					       const char *settingName,
					       T &result)
{
	obs_data_item_t *item = nullptr;

	for (item = obs_data_first(d); item; obs_data_item_next(&item)) {
		enum obs_data_type type = obs_data_item_gettype(item);
		const char *name = obs_data_item_get_name(item);
		if (strcmp(name, settingName) == 0) {
			if (!obs_data_item_has_user_value(item))
				continue;
			if (type == OBS_DATA_NUMBER) {
				enum obs_data_number_type type =
					obs_data_item_numtype(item);
				if (type == OBS_DATA_NUM_INT) {
					result = (int)obs_data_item_get_int(
						item);
				} else {
					result = (int)obs_data_item_get_double(
						item);
				}
			}
		}
	}
}

template<>
void OBSSourceManager::GetSettingValueWithName(obs_data_t *d,
					       const char *settingName,
					       std::string &result)
{
	obs_data_item_t *item = nullptr;

	for (item = obs_data_first(d); item; obs_data_item_next(&item)) {
		enum obs_data_type type = obs_data_item_gettype(item);
		const char *name = obs_data_item_get_name(item);
		if (strcmp(name, settingName) == 0) {
			if (!obs_data_item_has_user_value(item))
				continue;
			if (type == OBS_DATA_STRING) {
				result = std::string(
					obs_data_item_get_string(item));
			}
		}
	}
}

bool OBSSourceManager::VirtualCamAvailable()
{
	OBSDataAutoRelease privateData = obs_get_private_data();
	if (!privateData)
		return false;

	return obs_data_get_bool(privateData, "vcamEnabled");
}

bool OBSSourceManager::StartVirtualCamera()
{
	if (!VirtualCamAvailable()) {
		blog(LOG_ERROR, "Amdox virtual camera is not available.");
		return false;
	}

	if (obs_frontend_virtualcam_active()) {
		blog(LOG_INFO, "Amdox virtual camera was already running.");
		return false;
	}

	obs_frontend_start_virtualcam();

	return false;
}

bool OBSSourceManager::StopVirtualCamera()
{
	if (!VirtualCamAvailable()) {
		blog(LOG_ERROR, "Amdox virtual camera is not available.");
		return false;
	}

	if (!obs_frontend_virtualcam_active()) {
		blog(LOG_INFO, "Amdox virtual camera not started yet.");
		return false;
	}

	obs_frontend_stop_virtualcam();

	return true;
}

bool OBSSourceManager::SetStreamAddress(std::string &addr,
					std::string &username,
					std::string &passwd)
{
	if (addr.empty()) {
		blog(LOG_ERROR, "stream address invalide");
		return false;
	}

	OBSDataAutoRelease settings = obs_data_create();
	obs_data_set_string(settings, "server", addr.c_str());
	if (!username.empty() && !passwd.empty()) {
		obs_data_set_string(settings, "username", username.c_str());
		obs_data_set_string(settings, "password", passwd.c_str());
	}

	const std::string streamType = "rtmp_custom";

	OBSService currentStreamService = obs_frontend_get_streaming_service();
	std::string streamServiceType =
		obs_service_get_type(currentStreamService);
	if (streamServiceType != streamType) {
		// create new server
		OBSService newStreamService = obs_service_create(
			streamType.c_str(), "amdox_custom_service", settings,
			nullptr);
		if (!newStreamService) {
			blog(LOG_ERROR, "can not create stream server");
			return false;
		}

		obs_frontend_set_streaming_service(newStreamService);
	} else {
		// update server info
		OBSDataAutoRelease currentStreamServiceSettings =
			obs_service_get_settings(currentStreamService);
		OBSDataAutoRelease newStreamServiceSettings = obs_data_create();
		obs_data_apply(newStreamServiceSettings,
			       currentStreamServiceSettings);
		obs_data_apply(newStreamServiceSettings, settings);

		obs_service_update(currentStreamService,
				   newStreamServiceSettings);
	}

	// save the server
	obs_frontend_save_streaming_service();

	return true;
}

bool OBSSourceManager::StartStreaming()
{
	if (obs_frontend_streaming_active()) {
		blog(LOG_INFO, "already running streaming");
		return false;
	}

	obs_frontend_streaming_start();

	return true;
}

bool OBSSourceManager::StopStreaming()
{
	if (!obs_frontend_streaming_active()) {
		blog(LOG_INFO, "streaming not started yet");
		return false;
	}

	obs_frontend_streaming_stop();

	return true;
}

} //namespace accrecorder::manager

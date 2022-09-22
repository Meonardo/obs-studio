#pragma once

#include "obs-sources.h"

namespace accrecorder::manager {
class OBSSourceManager {
public:
	OBSSourceManager();
	~OBSSourceManager();

	bool IsMainSceneCreated() const;
	// rename the scene item
	bool Rename(source::SceneItem *item, std::string &newName);
	// attach the scene item to the main scene
	bool AttachSceneItem(source::SceneItem *item);
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

	// list all usb cameras
	void ListCameraItems(
		std::vector<std::shared_ptr<source::CameraSceneItem>> &items);

	// list all AudioInputDevices & AudioOutputDevices
	void ListAudioItems(
		std::vector<std::shared_ptr<source::AudioSceneItem>> &items,
		bool input = true);

	// remove scene item
	bool Remove(source::SceneItem *item);

	// start virtual camera
	bool StartVirtualCamera();
	// stop virtual camera
	bool StopVirtualCamera();
	// set stream address, like: rtmp://192.168.99.135
	bool SetStreamAddress(std::string &addr, std::string &username,
			      std::string &passwd);
	// start streaming
	bool StartStreaming();
	// stop streaming
	bool StopStreaming();

private:
	void LoadSceneItemFromScene(std::string &sceneName);
	void RemoveScene(std::string &name);
	obs_scene_t *CreateScene(std::string &name);

	template<typename T>
	static void GetSettingValueWithName(obs_data_t *d,
					 const char *settingName, T& result);
	template<>
	static void GetSettingValueWithName(obs_data_t *d,
					    const char *settingName, std::string &result);

	static obs_source_t *ValidateScene(std::string &name);
	static obs_source_t *ValidateInput(std::string &name);
	static bool VirtualCamAvailable();

	source::Scene *main_scene_;
};

} //namespace accrecorder::manager

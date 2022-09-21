#include "obs-sources.h"

struct CreateSceneItemData {
	obs_source_t *source;                             // In
	bool sceneItemEnabled;                            // In
	obs_transform_info *sceneItemTransform = nullptr; // In
	obs_sceneitem_crop *sceneItemCrop = nullptr;      // In
	OBSSceneItem sceneItem;                           // Out
};

static void CreateSceneItemHelper(void *_data, obs_scene_t *scene)
{
	auto *data = static_cast<CreateSceneItemData *>(_data);
	data->sceneItem = obs_scene_add(scene, data->source);

	if (data->sceneItemTransform)
		obs_sceneitem_set_info(data->sceneItem,
				       data->sceneItemTransform);

	if (data->sceneItemCrop)
		obs_sceneitem_set_crop(data->sceneItem, data->sceneItemCrop);

	obs_sceneitem_set_visible(data->sceneItem, data->sceneItemEnabled);
}

namespace accrecorder::source {

#pragma region Screen scene item
ScreenSceneItem::ScreenSceneItem(std::string &name)
	: name_(name),
	  index(0),
	  capture_method(0),
	  show_cursor(true),
	  should_apply_changes_(false)
{
	settings_.lock = false;
	settings_.pos.x = 0;
	settings_.pos.y = 0;
	settings_.hidden = false;
	settings_.scale.x = 1.0f;
	settings_.scale.y = 1.0f;
}

ScreenSceneItem::~ScreenSceneItem() {}

uint64_t ScreenSceneItem::SceneID() const
{
	return scene_id_;
}

void ScreenSceneItem::SetSceneID(uint64_t id)
{
	scene_id_ = id;
}

std::string ScreenSceneItem::Name() const
{
	return name_;
}

void ScreenSceneItem::SetName(std::string &name)
{
	if (name == name_)
		return;
	name_ = name;
}

std::string ScreenSceneItem::Kind() const
{
	return "monitor_capture";
}

SceneItem::Type ScreenSceneItem::type() const
{
	return kScreen;
}

Scene *ScreenSceneItem::scene() const
{
	return nullptr;
}

SceneItem::Settings ScreenSceneItem::GetSettings() const
{
	return settings_;
}

void ScreenSceneItem::UpdateSettings(SceneItem::Settings settings)
{
	vec2_copy(&settings_.pos, &settings.pos);
	vec2_copy(&settings_.scale, &settings.scale);
	settings_.hidden = settings.hidden;
	settings_.lock = settings.lock;

	should_apply_changes_ = true;
}

void ScreenSceneItem::Hide(bool hidden)
{
	if (hidden == settings_.hidden)
		return;
	settings_.hidden = hidden;
	should_apply_changes_ = true;
}

void ScreenSceneItem::Lock(bool lock)
{
	if (lock == settings_.lock)
		return;
	settings_.lock = lock;
	should_apply_changes_ = true;
}

void ScreenSceneItem::UpdateScale(vec2 scale)
{
	if (scale.x == settings_.scale.x && scale.y == settings_.scale.y) {
		return;
	}
	vec2_copy(&settings_.scale, &scale);
	should_apply_changes_ = true;
}

void ScreenSceneItem::UpdatePosition(vec2 pos)
{
	if (pos.x == settings_.pos.x && pos.y == settings_.pos.y) {
		return;
	}
	vec2_copy(&settings_.scale, &pos);
	should_apply_changes_ = true;
}

bool ScreenSceneItem::ShouldApplyAnyUpdates() const
{
	return should_apply_changes_;
}

void ScreenSceneItem::MarkUpdateCompleted()
{
	should_apply_changes_ = false;
}

obs_data_t *ScreenSceneItem::Properties() const
{
	obs_data_t *data = obs_data_create();
	obs_data_set_int(data, "monitor", index);
	obs_data_set_int(data, "method", 0);
	obs_data_set_bool(data, "cursor", show_cursor);

	return data;
}

#pragma endregion Sceen item

#pragma region IPCamera item
IPCameraSceneItem::IPCameraSceneItem(std::string &name, std::string &url,
				     bool stopOnHide)
	: name_(name),
	  url_(url),
	  stop_on_hide_(stopOnHide),
	  should_apply_changes_(false)
{
	settings_.lock = false;
	settings_.pos.x = 0;
	settings_.pos.y = 0;
	settings_.hidden = false;
	settings_.scale.x = 1.0f;
	settings_.scale.y = 1.0f;
}

IPCameraSceneItem::~IPCameraSceneItem() {}

uint64_t IPCameraSceneItem::SceneID() const
{
	return scene_id_;
}

void IPCameraSceneItem::SetSceneID(uint64_t id)
{
	scene_id_ = id;
}

std::string IPCameraSceneItem::Name() const
{
	return name_;
}

void IPCameraSceneItem::SetName(std::string &name)
{
	if (name == name_)
		return;
	name_ = name;
}

std::string IPCameraSceneItem::Kind() const
{
	return "gstreamer-source";
}

SceneItem::Type IPCameraSceneItem::type() const
{
	return kIPCamera;
}

Scene *IPCameraSceneItem::scene() const
{
	return nullptr;
}

SceneItem::Settings IPCameraSceneItem::GetSettings() const
{
	return settings_;
}

void IPCameraSceneItem::UpdateSettings(SceneItem::Settings settings)
{
	vec2_copy(&settings_.pos, &settings.pos);
	vec2_copy(&settings_.scale, &settings.scale);
	settings_.hidden = settings.hidden;
	settings_.lock = settings.lock;

	should_apply_changes_ = true;
}

void IPCameraSceneItem::Hide(bool hidden)
{
	if (hidden == settings_.hidden)
		return;
	settings_.hidden = hidden;
	should_apply_changes_ = true;
}

void IPCameraSceneItem::Lock(bool lock)
{
	if (lock == settings_.lock)
		return;
	settings_.lock = lock;
	should_apply_changes_ = true;
}

void IPCameraSceneItem::UpdateScale(vec2 scale)
{
	if (scale.x == settings_.scale.x && scale.y == settings_.scale.y) {
		return;
	}
	vec2_copy(&settings_.scale, &scale);
	should_apply_changes_ = true;
}

void IPCameraSceneItem::UpdatePosition(vec2 pos)
{
	if (pos.x == settings_.pos.x && pos.y == settings_.pos.y) {
		return;
	}
	vec2_copy(&settings_.scale, &pos);
	should_apply_changes_ = true;
}

void IPCameraSceneItem::UpdateURL(std::string &url)
{
	if (url.empty() || url == url_) {
		return;
	}
	url_ = url;
	should_apply_changes_ = true;
}

void IPCameraSceneItem::UpdateStopOnHide(bool state)
{
	if (stop_on_hide_ == state)
		return;
	stop_on_hide_ = state;
	should_apply_changes_ = true;
}

bool IPCameraSceneItem::ShouldApplyAnyUpdates() const
{
	return should_apply_changes_;
}

void IPCameraSceneItem::MarkUpdateCompleted()
{
	should_apply_changes_ = false;
}

obs_data_t *IPCameraSceneItem::Properties() const
{
	obs_data_t *data = obs_data_create();
	const size_t count = 512;
	char pipeline[count];
	snprintf(pipeline, count,
		 "uridecodebin uri=%s name=bin ! queue ! video.", url_.c_str());

	obs_data_set_string(data, "pipeline", pipeline);
	obs_data_set_bool(data, "sync_appsink_audio", false);
	obs_data_set_bool(data, "sync_appsink_video", false);
	obs_data_set_bool(data, "disable_async_appsink_video", true);
	obs_data_set_bool(data, "disable_async_appsink_audio", true);
	obs_data_set_bool(data, "stop_on_hide", stop_on_hide_);

	return data;
}
#pragma endregion IPCamera item

#pragma region Camera item
CameraSceneItem::CameraSceneItem(std::string &name)
	: name_(name), should_apply_changes_(false)
{
	settings_.lock = false;
	settings_.pos.x = 0;
	settings_.pos.y = 0;
	settings_.hidden = false;
	settings_.scale.x = 1.0f;
	settings_.scale.y = 1.0f;
}

CameraSceneItem::~CameraSceneItem() {}

uint64_t CameraSceneItem::SceneID() const
{
	return scene_id_;
}

void CameraSceneItem::SetSceneID(uint64_t id)
{
	scene_id_ = id;
}

std::string CameraSceneItem::Name() const
{
	return name_;
}

void CameraSceneItem::SetName(std::string &name)
{
	if (name == name_)
		return;
	name_ = name;
}

std::string CameraSceneItem::Kind() const
{
	return "dshow_input";
}

SceneItem::Type CameraSceneItem::type() const
{
	return kCamera;
}

Scene *CameraSceneItem::scene() const
{
	return nullptr;
}

SceneItem::Settings CameraSceneItem::GetSettings() const
{
	return settings_;
}

void CameraSceneItem::UpdateSettings(SceneItem::Settings settings)
{
	vec2_copy(&settings_.pos, &settings.pos);
	vec2_copy(&settings_.scale, &settings.scale);
	settings_.hidden = settings.hidden;
	settings_.lock = settings.lock;

	should_apply_changes_ = true;
}

void CameraSceneItem::Hide(bool hidden)
{
	if (hidden == settings_.hidden)
		return;
	settings_.hidden = hidden;
	should_apply_changes_ = true;
}

void CameraSceneItem::Lock(bool lock)
{
	if (lock == settings_.lock)
		return;
	settings_.lock = lock;
	should_apply_changes_ = true;
}

void CameraSceneItem::UpdateScale(vec2 scale)
{
	if (scale.x == settings_.scale.x && scale.y == settings_.scale.y) {
		return;
	}
	vec2_copy(&settings_.scale, &scale);
	should_apply_changes_ = true;
}

void CameraSceneItem::UpdatePosition(vec2 pos)
{
	if (pos.x == settings_.pos.x && pos.y == settings_.pos.y) {
		return;
	}
	vec2_copy(&settings_.pos, &pos);
	should_apply_changes_ = true;
}

bool CameraSceneItem::ShouldApplyAnyUpdates() const
{
	return should_apply_changes_;
}

void CameraSceneItem::MarkUpdateCompleted()
{
	should_apply_changes_ = false;
}

bool CameraSceneItem::SelectResolution(uint32_t idx)
{
	if (idx >= resolutions_.size()) {
		return false;
	}
	selected_res_ = resolutions_[idx];

	return true;
}

bool CameraSceneItem::SelectFps(uint32_t idx)
{
	if (idx >= fps_.size()) {
		return false;
	}
	selected_fps_ = fps_[idx];

	return true;
}

void CameraSceneItem::GetAvailableResolutions(
	std::vector<std::string> &res) const
{
	// make copy of the resolutions_
	for (auto r : resolutions_) {
		res.emplace_back(std::move(r));
	}
}

void CameraSceneItem::GetAvailableFps(
	std::vector<std::tuple<std::string, int64_t>> &fps) const
{
	// make copy of the fps_
	for (auto r : fps_) {
		fps.emplace_back(std::move(r));
	}
}

obs_data_t *CameraSceneItem::Properties() const
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "video_device_id", device_id_.c_str());
	//resolution
	obs_data_set_string(data, "resolution", selected_res_.c_str());
	// res_type
	obs_data_set_int(data, "res_type", 1);
	// must set last-device/id
	obs_data_set_string(data, "last_resolution", selected_res_.c_str());
	obs_data_set_string(data, "last_video_device_id", device_id_.c_str());
	// frame_interval
	obs_data_set_int(data, "frame_interval", std::get<1>(selected_fps_));

	return data;
}
#pragma endregion Camera item

#pragma region Audio item
AudioSceneItem::AudioSceneItem(std::string &name)
	: name_(name), should_apply_changes_(false)
{
	settings_.lock = false;
	settings_.pos.x = 0;
	settings_.pos.y = 0;
	settings_.hidden = false;
	settings_.scale.x = 0.f;
	settings_.scale.y = 0.f;
}

AudioSceneItem::~AudioSceneItem() {}

uint64_t AudioSceneItem::SceneID() const
{
	return scene_id_;
}

void AudioSceneItem::SetSceneID(uint64_t id)
{
	scene_id_ = id;
}

std::string AudioSceneItem::Name() const
{
	return name_;
}

void AudioSceneItem::SetName(std::string &name)
{
	if (name == name_)
		return;
	name_ = name;
}

Scene *AudioSceneItem::scene() const
{
	return nullptr;
}

SceneItem::Settings AudioSceneItem::GetSettings() const
{
	return settings_;
}

void AudioSceneItem::UpdateSettings(SceneItem::Settings settings)
{
	settings_.hidden = settings.hidden;
	settings_.lock = settings.lock;

	should_apply_changes_ = true;
}

void AudioSceneItem::Hide(bool hidden)
{
	if (hidden == settings_.hidden)
		return;
	settings_.hidden = hidden;
	should_apply_changes_ = true;
}

void AudioSceneItem::Lock(bool lock)
{
	if (lock == settings_.lock)
		return;
	settings_.lock = lock;
	should_apply_changes_ = true;
}

bool AudioSceneItem::ShouldApplyAnyUpdates() const
{
	return should_apply_changes_;
}

void AudioSceneItem::MarkUpdateCompleted()
{
	should_apply_changes_ = false;
}

obs_data_t *AudioSceneItem::Properties() const
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "device_id", device_id_.c_str());
	return data;
}

////////////////////////////////////////////////////////////////////////////////////
// AudioInputItem subclass of AudioSceneItem
AudioInputItem::AudioInputItem(std::string &name) : AudioSceneItem(name) {}

AudioInputItem::~AudioInputItem() {}

std::string AudioInputItem::Kind() const
{
	return "wasapi_input_capture";
}

SceneItem::Type AudioInputItem::type() const
{
	return kAudioInput;
}

////////////////////////////////////////////////////////////////////////////////////
// AudioOutputItem subclass of AudioSceneItem
AudioOutputItem::AudioOutputItem(std::string &name) : AudioSceneItem(name) {}

AudioOutputItem::~AudioOutputItem() {}

std::string AudioOutputItem::Kind() const
{
	return "wasapi_output_capture";
}

SceneItem::Type AudioOutputItem::type() const
{
	return kAudioOutput;
}

#pragma endregion Audio item

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

Scene::Scene(std::string &name, obs_scene_t *src) : name_(name), scene_(src) {}

Scene::~Scene()
{
	obs_scene_release(scene_);
}

bool Scene::Attach(SceneItem *item)
{
	obs_source_t *ret = obs_get_source_by_name(item->Name().c_str());
	if (ret) {
		blog(LOG_ERROR, "source with name %s already attached!",
		     item->Name().c_str());
		return false;
	}

	OBSDataAutoRelease inputSettings = item->Properties();
	// create the input source
	OBSSourceAutoRelease input = obs_source_create(item->Kind().c_str(),
						       item->Name().c_str(),
						       inputSettings, nullptr);
	if (!input) {
		blog(LOG_ERROR, "create scene source failed!");
		return false;
	}

	uint32_t flags = obs_source_get_output_flags(input);
	if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0)
		obs_source_set_monitoring_type(
			input, OBS_MONITORING_TYPE_MONITOR_ONLY);

	// create a scene item for the input
	OBSSceneItemAutoRelease sceneItem =
		CreateSceneItem(input, scene_, true, NULL, NULL);
	if (sceneItem == nullptr) {
		blog(LOG_ERROR, "create scene item failed!");
		return false;
	}

	// save scene id
	item->SetSceneID(obs_sceneitem_get_id(sceneItem));

	// save item to vector
	items_.push_back(item);

	return true;
}

bool Scene::Detach(SceneItem *item)
{
	if (item == nullptr)
		return false;

	items_.erase(std::remove_if(items_.begin(), items_.end(),
				    [item](const SceneItem *scene_item) {
					    return scene_item == item;
				    }),
		     items_.end());
	return false;
}

bool Scene::ApplySceneItemSettingsUpdate(SceneItem *item)
{
	if (!item->ShouldApplyAnyUpdates()) {
		blog(LOG_INFO, "nothing changed, no need to update.");
		return false;
	}

	// dont use autorelease!!!
	obs_sceneitem_t *sceneItem =
		obs_scene_find_sceneitem_by_id(scene_, item->SceneID());
	if (sceneItem == nullptr) {
		blog(LOG_ERROR, "can not find the scene item in the scene!");
		return false;
	}

	obs_sceneitem_defer_update_begin(sceneItem);
	// visability
	obs_sceneitem_set_visible(sceneItem, !item->GetSettings().hidden);
	// transform
	vec2 newPosition = item->GetSettings().pos;
	obs_sceneitem_set_pos(sceneItem, &newPosition);
	obs_sceneitem_set_alignment(sceneItem, 5);
	// scale
	vec2 newScale = item->GetSettings().scale;
	obs_sceneitem_set_scale(sceneItem, &newScale);
	// lock
	obs_sceneitem_set_locked(sceneItem, item->GetSettings().lock);
	obs_sceneitem_defer_update_end(sceneItem);

	// mark updates completed
	item->MarkUpdateCompleted();

	return true;
}

obs_sceneitem_t *Scene::CreateSceneItem(obs_source_t *source,
					obs_scene_t *scene,
					bool sceneItemEnabled,
					obs_transform_info *sceneItemTransform,
					obs_sceneitem_crop *sceneItemCrop)
{
	if (!(source && scene))
		return nullptr;

	// Create data struct and populate for scene item creation
	CreateSceneItemData data;
	data.source = source;
	data.sceneItemEnabled = sceneItemEnabled;
	data.sceneItemTransform = sceneItemTransform;
	data.sceneItemCrop = sceneItemCrop;

	// Enter graphics context and create the scene item
	obs_enter_graphics();
	obs_scene_atomic_update(scene, CreateSceneItemHelper, &data);
	obs_leave_graphics();

	obs_sceneitem_addref(data.sceneItem);

	return data.sceneItem;
}

} // namespace accrecorder::source

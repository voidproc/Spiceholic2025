#include "AppSetting.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	AppSetting::AppSetting()
	{
	}

	void AppSetting::load()
	{
		const JSON json = JSON::Load(Resource(AppSettingJsonPath));
		if (not json) return;

		settings_.title = json[U"Title"].getString();
		settings_.version = json[U"Version"].getString();

#ifdef DEBUG_MODE
		settings_.debug.startStageID = json[U"Debug"][U"StartStageID"].getString();
#endif

		for (const auto& s : json[U"Sprite"])
		{
			SpriteInfo sprite;
			sprite.textureName = s.value[U"Texture"].getString();
			sprite.count = s.value[U"Count"].get<int>();
			sprite.size = s.value[U"Size"].get<int>();
			sprite.pos = Point{ s.value[U"Pos"][0].get<int>(), s.value[U"Pos"][1].get<int>() };

			settings_.sprite[s.key] = sprite;
		}

		Array<String> subtitles;
		for (auto&& [index, object] : json[U"StageSubtitle"])
		{
			subtitles.push_back(object.getString());
		}
		for (const auto& object : json[U"StageSubtitleMap"])
		{
			settings_.stageSubtitle[object.key] = subtitles[object.value.get<int>()];
		}

		for (const auto& object : json[U"StageGroundMap"])
		{
			settings_.stageGroundTexture[object.key] = object.value.getString();
		}

	}

	const AppSetting::Settings& AppSetting::get() const
	{
		return settings_;
	}

	AppSetting::Settings& AppSetting::get()
	{
		return settings_;
	}
}

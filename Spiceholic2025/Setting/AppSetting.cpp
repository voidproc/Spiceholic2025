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

		settings_.debug_startStageID = json[U"Debug_StartStageID"].getString();

		for (const auto& s : json[U"Sprite"])
		{
			SpriteInfo sprite;
			sprite.textureName = s.value[U"Texture"].getString();
			sprite.count = s.value[U"Count"].get<int>();
			sprite.size = s.value[U"Size"].get<int>();
			sprite.pos = Point{ s.value[U"Pos"][0].get<int>(), s.value[U"Pos"][1].get<int>() };

			settings_.sprite[s.key] = sprite;
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

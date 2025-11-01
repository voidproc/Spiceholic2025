#include "AppSetting.h"
#include "Config/GameConfig.h"
#include "Stage/StageGroup.h"

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
		settings_.debug.wait = json[U"Debug"][U"Wait"].get<int>();
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

		Array<StageGroupInfo> stageGroups;
		for (auto&& [index, object] : json[U"StageGroupInfo"])
		{
			StageGroupInfo info;
			info.group = ToEnum<StageGroupType>(static_cast<uint32>(stageGroups.size()));
			if (FromEnum(info.group) == 4) info.group = StageGroupType::Magma;
			info.subtitle = object[U"Subtitle"].getString();
			info.groundTexture = object[U"Ground"].getString();
			info.bgm = object[U"BGM"].getString();
			stageGroups.push_back(info);
		}

		for (const auto& object : json[U"StageGroupMap"])
		{
			settings_.stageGroupInfo[object.key] = stageGroups[object.value.get<int>()];
		}

		for (int i = 0; i < json[U"StageList"].size(); ++i)
		{
			settings_.stageList.push_back(json[U"StageList"][i].getString());
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

#pragma once

namespace Spiceholic
{
	struct SpriteInfo
	{
		String textureName;
		int count;
		int size;
		Point pos;
	};

	struct StageGroupInfo;

	class AppSetting
	{
	public:
		struct Settings
		{
			String title;
			String version;
			HashTable<String, SpriteInfo> sprite;
			HashTable<String, StageGroupInfo> stageGroupInfo;
			Array<String> stageList;

			struct
			{
				String startStageID;
				int wait;
			}
			debug;
		};

		AppSetting();

		void load();

		const Settings& get() const;

		Settings& get();

	private:
		Settings settings_;
	};
}

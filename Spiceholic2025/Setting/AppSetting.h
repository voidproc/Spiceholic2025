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

	struct StageGroupInfo
	{
		String subtitle;
		String groundTexture;
	};

	class AppSetting
	{
	public:
		struct Settings
		{
			String title;
			String version;
			HashTable<String, SpriteInfo> sprite;
			HashTable<String, StageGroupInfo> stageGroupInfo;
			//HashTable<String, String> stageSubtitle;
			//HashTable<String, String> stageGroundTexture;

			struct
			{
				String startStageID;
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

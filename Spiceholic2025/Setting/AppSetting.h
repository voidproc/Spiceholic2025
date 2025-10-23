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

	class AppSetting
	{
	public:
		struct Settings
		{
			String title;
			String version;
			HashTable<String, SpriteInfo> sprite;

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

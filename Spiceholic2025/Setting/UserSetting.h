#pragma once

namespace Spiceholic
{
	// ユーザー設定項目
	enum class UserSettingField
	{
		WindowScale,
		SEVolume,
		BGMVolume,
		UseEffect,
		Back,

		None,
	};

	// ユーザー設定項目の名称
	StringView GetUserSettingFieldName(UserSettingField field);


	class UserSetting
	{
	public:
		struct Settings
		{
			int32 windowScale;
			int32 seVolume;
			int32 bgmVolume;
			bool useEffect;
		};

		UserSetting();

		void load(FilePathView path);

		void save(FilePathView path);

		const Settings& get() const;

		Settings& get();

	private:
		Settings settings_;
	};
}

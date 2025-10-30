#include "UserSetting.h"

namespace Spiceholic
{
	StringView GetUserSettingFieldName(UserSettingField field)
	{
		switch (field)
		{
		case UserSettingField::WindowScale: return U"Window Scale";
		case UserSettingField::SEVolume: return U"SE Volume";
		case UserSettingField::BGMVolume: return U"BGM Volume";
		case UserSettingField::UseEffect: return U"Use Effect";
		case UserSettingField::Back: return U"Back";
		}

		return U""_sv;
	}

	namespace
	{
		constexpr UserSetting::Settings DefaultSettings{
			.windowScale = 3,
			.seVolume = 80,
			.bgmVolume = 80,
			.useEffect = true,
		};
	}

	UserSetting::UserSetting()
		: settings_{ DefaultSettings }
	{
	}

	void UserSetting::load(FilePathView path)
	{
		INI ini{ path };
		if (not ini) return;

		settings_.windowScale = ini.getOr<int32>(U"", U"WindowScale", DefaultSettings.windowScale);
		settings_.seVolume = ini.getOr<int32>(U"", U"SE_Volume", DefaultSettings.seVolume);
		settings_.bgmVolume = ini.getOr<int32>(U"", U"BGM_Volume", DefaultSettings.bgmVolume);
		settings_.useEffect = ini.getOr<bool>(U"", U"UseEffect", DefaultSettings.useEffect);
	}

	void UserSetting::save(FilePathView path)
	{
		INI ini{};

		ini.writeGlobal(U"WindowScale", settings_.windowScale);
		ini.writeGlobal(U"SE_Volume", settings_.seVolume);
		ini.writeGlobal(U"BGM_Volume", settings_.bgmVolume);
		ini.writeGlobal(U"UseEffect", settings_.useEffect);

		ini.save(path);
	}

	const UserSetting::Settings& UserSetting::get() const
	{
		return settings_;
	}

	UserSetting::Settings& UserSetting::get()
	{
		return settings_;
	}

}

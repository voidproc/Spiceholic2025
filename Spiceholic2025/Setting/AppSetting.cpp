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
		settings_.dummy = json[U"Dummy"].getOr<int32>(0);
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

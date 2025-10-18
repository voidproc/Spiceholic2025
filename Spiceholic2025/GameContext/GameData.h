#pragma once

namespace Spiceholic
{
	class AppSetting;

	struct GameData
	{
		std::unique_ptr<AppSetting> appSetting;

		GameData();
	};
}

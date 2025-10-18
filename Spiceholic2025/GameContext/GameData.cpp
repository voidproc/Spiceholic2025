#include "GameData.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	GameData::GameData()
		:
		appSetting{ std::make_unique<AppSetting>() }
	{
	};
}

#include "GameData.h"
#include "Actor/Player.h"
#include "Actor/Actor.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	GameData::GameData()
		:
		appSetting{ std::make_unique<AppSetting>() },
		player{},
		actors{ Arg::reserve = 128 }
	{
	}

	GameData::~GameData()
	{
	}
}

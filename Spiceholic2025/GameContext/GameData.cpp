#include "GameData.h"
#include "Actor/Actor.h"
#include "Actor/Block.h"
#include "Actor/Player.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	GameData::GameData()
		:
		appSetting{ std::make_unique<AppSetting>() },
		player{},
		blocks{ Arg::reserve = 128 },
		actors{ Arg::reserve = 128 }
	{
	}

	GameData::~GameData()
	{
	}
}

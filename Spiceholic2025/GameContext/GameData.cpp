#include "GameData.h"
#include "Actor/Actor.h"
#include "Actor/Block.h"
#include "Actor/Player.h"
#include "Input/ActionInput.h"
#include "Setting/AppSetting.h"
#include "Setting/UserSetting.h"

namespace Spiceholic
{
	GameData::GameData()
		:
		appSetting{ std::make_unique<AppSetting>() },
		userSetting{ std::make_unique<UserSetting>() },
		actionInput{ std::make_unique<ActionInput>() },
		player{},
		blocks{ Arg::reserve = 128 },
		actors{ Arg::reserve = 128 }
	{
	}

	GameData::~GameData()
	{
	}
}

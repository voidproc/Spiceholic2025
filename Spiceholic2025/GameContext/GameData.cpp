#include "GameData.h"
#include "Actor/Actor.h"
#include "Actor/Block.h"
#include "Actor/Player.h"
#include "Core/Gauge.h"
#include "Core/Score.h"
#include "Input/ActionInput.h"
#include "Setting/AppSetting.h"
#include "Setting/UserSetting.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
	GameData::GameData()
		:
		appSetting{ std::make_unique<AppSetting>() },
		userSetting{ std::make_unique<UserSetting>() },
		actionInput{ std::make_unique<ActionInput>() },
		stageData{ std::make_unique<StageData>() },
		nextStageID{ U"1" },
		gauge{ std::make_unique<Gauge>() },
		score{ std::make_unique<Score>() },
		player{},
		blocks{ Arg::reserve = 128 },
		actors{ Arg::reserve = 128 },
		titleCharacterShown{ false }
	{
	}

	GameData::~GameData()
	{
	}
}

#pragma once

#include "Actor/ActorType.h"

namespace Spiceholic
{
	namespace
	{
		constexpr std::array<ActorType, 65> TileID_ActorType_Map =
		{
			ActorType::None,

			ActorType::PlayerA,
			ActorType::BlockSteel,
			ActorType::BlockCanBreak,
			ActorType::BlockCanBreakGray,
			ActorType::BlockSpike,
			ActorType::None,
			ActorType::CameraTopLeft,
			ActorType::CameraBottomRight,

			ActorType::ItemChilipepper,
			ActorType::ItemKey,
			ActorType::ItemCurry,
			ActorType::ItemHabanero,
			ActorType::ItemMapo,
			ActorType::ItemKeyEnd,
			ActorType::None,
			ActorType::None,

			ActorType::EnemyChick,
			ActorType::EnemyChickFly,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
		};
	}

	ActorType GetActorTypeFromTileID(int tileID)
	{
		return TileID_ActorType_Map[tileID];
	}
}

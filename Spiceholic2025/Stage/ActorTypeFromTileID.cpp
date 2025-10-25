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
			ActorType::None,
			ActorType::None,
			ActorType::CameraTopLeft,
			ActorType::CameraBottomRight,

			ActorType::ItemChilipepper,
			ActorType::ItemKey,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,
			ActorType::None,

			ActorType::EnemyChick,
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
			ActorType::None,
		};
	}

	ActorType GetActorTypeFromTileID(int tileID)
	{
		return TileID_ActorType_Map[tileID];
	}
}

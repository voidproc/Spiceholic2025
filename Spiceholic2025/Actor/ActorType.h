#pragma once

namespace Spiceholic
{
	enum class ActorType
	{
		None,

		// Player:
		PlayerA,
		//...

		// Enemy:
		EnemyChick,
		EnemyChickFly,
		//...

		// Item:
		ItemChilipepper,
		ItemKey,
		ItemKeyEnd,
		ItemCurry,
		ItemHabanero,
		ItemMapo,
		ItemWasabi,
		//...

		// Weapon:
		WeaponFire,
		WeaponExplode,
		//...

		// Block:
		BlockSteel,
		BlockCanBreak,
		BlockCanBreakGray,
		BlockSpike,
		BlockGiftbox,
		//...

		// Effect:
		FxNotice,

		// Other:
		CameraTopLeft,
		CameraBottomRight,

	};
}

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
		ItemCurry,
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

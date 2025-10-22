#pragma once

namespace Spiceholic
{
	enum class Direction
	{
		Up,
		Right,
		Down,
		Left,

		None,
	};

	double DirectionToAngle(Direction d);

	Direction StringToDirection(const String& d);

}

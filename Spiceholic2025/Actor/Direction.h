#pragma once

namespace Spiceholic
{
	enum class Direction
	{
		N,
		E,
		S,
		W,
	};

	double DirectionToAngle(Direction d);
}

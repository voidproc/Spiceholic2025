#include "Direction.h"

namespace Spiceholic
{
	double DirectionToAngle(Direction d)
	{
		switch (d)
		{
		case Direction::Up: return 0_deg;
		case Direction::Right: return 90_deg;
		case Direction::Down: return 180_deg;
		case Direction::Left: return 270_deg;
		}

		return 0;
	}

	Direction StringToDirection(const String& d)
	{
		if (d == U"Up") return Direction::Up;
		if (d == U"Right") return Direction::Right;
		if (d == U"Down") return Direction::Down;
		if (d == U"Left") return Direction::Left;

		return Direction::None;
	}
}

#include "Direction.h"

double Spiceholic::DirectionToAngle(Direction d)
{
	switch (d)
	{
	case Direction::N: return 0_deg;
	case Direction::E: return 90_deg;
	case Direction::S: return 180_deg;
	case Direction::W: return 270_deg;
	}

    return 0;
}

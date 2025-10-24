#include "Color.h"

namespace Spiceholic
{
	ColorF GaugeMaxEffectColor()
	{
		return Palette::Red.lerp(Palette::Yellow, 0.6 * Periodic::Square0_1(0.07s, ClockTime()));
	}
}

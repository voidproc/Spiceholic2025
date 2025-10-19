#include "Periodic.h"
#pragma once

namespace Spiceholic
{
	int PeriodicStair(const Duration& period, int minVal, int maxVal, double time)
	{
		return minVal + Clamp<int>(static_cast<int>((maxVal - minVal + 1) * Periodic::Sawtooth0_1(period, time)), 0, maxVal);
	}
}

#pragma once

namespace Spiceholic
{
	class GameClock;

	namespace GlobalClock
	{
		void Init();

		void Pause();

		bool IsPaused();

		void Start();

		double Time();

		ISteadyClock* Get();
	}

	ISteadyClock* Clock();

	double ClockTime();
}

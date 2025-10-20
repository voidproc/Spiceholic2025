#include "GlobalClock.h"
#include "GameClock.h"

namespace Spiceholic
{
	namespace GlobalClock
	{
		namespace
		{
			Optional<GameClock> gGameClock;
		}

		void Init()
		{
			gGameClock = GameClock{ StartImmediately::Yes };
		}

		void Pause()
		{
			gGameClock->pause();
		}

		bool IsPaused()
		{
			return gGameClock->isPaused();
		}

		void Start()
		{
			gGameClock->start();
		}

		double Time()
		{
			return gGameClock->getSec();
		}

		ISteadyClock* Get()
		{
			return &gGameClock.value();
		}
	}

	ISteadyClock* Clock()
	{
		return GlobalClock::Get();
	}

	double ClockTime()
	{
		return GlobalClock::Time();
	}
}

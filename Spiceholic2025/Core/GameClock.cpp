#include "GameClock.h"

namespace Spiceholic
{
	GameClock::GameClock(StartImmediately startImmediately)
		: m_clock{ startImmediately }
	{
	}

	GameClock::~GameClock() = default;

	uint64 GameClock::getMicrosec()
	{
		return m_clock.us64();
	}

	void GameClock::start()
	{
		m_clock.start();
	}

	void GameClock::pause()
	{
		m_clock.pause();
	}

	bool GameClock::isStarted() const
	{
		return m_clock.isStarted();
	}

	bool GameClock::isPaused() const
	{
		return m_clock.isPaused();
	}

	double GameClock::getSec() const
	{
		return m_clock.sF();
	}
}

#pragma once

namespace Spiceholic
{
	class GameClock : public ISteadyClock
	{
	public:
		GameClock(StartImmediately startImmediately = StartImmediately::No);

		~GameClock() override;

		uint64 getMicrosec() override;

		void start();

		void pause();

		bool isStarted() const;

		bool isPaused() const;

		double getSec() const;

		auto isPausedFunc() const
		{
			return [&]() { return isPaused(); };
		}

	private:
		VariableSpeedStopwatch m_clock;
	};


}

#pragma once

namespace Spiceholic
{
	class Menu
	{
	public:
		Menu(size_t itemCount, const Duration& moveCursorTimerDuration = 0.1s, const Duration& decideTimerDuration = 0.8s);

		void selectPrevious();
		void selectNext();
		void setSelectedIndex(size_t index);
		size_t selectedIndex() const;
		const Timer& moveCursorTimer() const;



		void decide();
		void resetDecideTimer();
		const Timer& decideTimer() const;


	private:
		size_t itemCount_;
		size_t selectedIndex_;
		Timer timerMoveCursor_;
		Timer timerDecide_;
	};

	Vec2 MenuMoveCursorVibrateVertical(const Menu& menu);
}

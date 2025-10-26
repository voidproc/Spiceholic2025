#include "Menu.h"

namespace Spiceholic
{
	Menu::Menu(size_t itemCount, const Duration& moveCursorTimerDuration, const Duration& decideTimerDuration)
		:
		itemCount_{ itemCount },
		selectedIndex_{ 0 },
		timerMoveCursor_{ moveCursorTimerDuration, StartImmediately::No },
		timerDecide_{ decideTimerDuration, StartImmediately::No }
	{
	}

	void Menu::selectPrevious()
	{
		selectedIndex_ = (selectedIndex_ + itemCount_ - 1) % itemCount_;

		timerMoveCursor_.restart();
	}

	void Menu::selectNext()
	{
		selectedIndex_ = (selectedIndex_ + itemCount_ + 1) % itemCount_;

		timerMoveCursor_.restart();
	}

	void Menu::setSelectedIndex(size_t index)
	{
		selectedIndex_ = index;
	}

	void Menu::decide()
	{
		timerDecide_.restart();
	}

	void Menu::resetDecideTimer()
	{
		timerDecide_.reset();
	}

	const Timer& Menu::decideTimer() const
	{
		return timerDecide_;
	}

	size_t Menu::selectedIndex() const
	{
		return selectedIndex_;
	}

	const Timer& Menu::moveCursorTimer() const
	{
		return timerMoveCursor_;
	}

	Vec2 MenuMoveCursorVibrateVertical(const Menu& menu)
	{
		return Vec2{ 0, menu.moveCursorTimer().isRunning() * 2 * Periodic::Sine1_1(0.01s) };
	}
}

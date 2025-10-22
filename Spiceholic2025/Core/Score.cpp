#include "Score.h"

namespace Spiceholic
{
	namespace
	{
		constexpr Duration AnimationDuration = 0.3s;
	}

	Score::Score()
		:
		currentScore_{ 0 },
		hiscore_{ 0 },
		animationTimer_{},
		animationStartScore_{ 0 }
	{
	}

	void Score::set(int32 value)
	{
		startAnimation_(currentScore_);

		currentScore_ = value;

		if (hiscore_ < currentScore_)
		{
			setHiscore(currentScore_);
		}
	}

	void Score::add(int32 value)
	{
		startAnimation_(currentScore_);

		currentScore_ += value;

		if (hiscore_ < currentScore_)
		{
			setHiscore(currentScore_);
		}
	}

	int32 Score::currentScore() const
	{
		return currentScore_;
	}

	Score::operator int32() const
	{
		return currentScore();
	}

	void Score::setHiscore(int32 value)
	{
		hiscore_ = value;
	}

	int32 Score::hiscore() const
	{
		return hiscore_;
	}

	int32 Score::displayScore() const
	{
		return animationStartScore_ +
			static_cast<int32>((currentScore_ - animationStartScore_) * animationTimer_.progress0_1());
	}

	void Score::startAnimation_(int32 startScore)
	{
		animationStartScore_ = startScore;
		animationTimer_.restart(AnimationDuration);
	}
}

#pragma once

namespace Spiceholic
{
	class Score
	{
	public:
		[[nodiscard]]
		Score();

		void set(int32 value);

		void add(int32 value);

		int32 currentScore() const;

		operator int32() const;

		void setHiscore(int32 value);

		int32 hiscore() const;

		int32 displayScore() const;

		bool animating() const;

	private:
		void startAnimation_(int32 startScore);

		int32 currentScore_;
		int32 hiscore_;

		Timer animationTimer_;
		int32 animationStartScore_;
	};
}

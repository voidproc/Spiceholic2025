#include "Position.h"

namespace Spiceholic
{
	Position::Position()
		:
		Position{ Vec2{} }
	{
	}

	Position::Position(const Vec2& pos)
		:
		currentPos_{ pos },
		proposedPos_{ pos }
	{
	}

	const Vec2& Position::proposedPos() const
	{
		return proposedPos_;
	}

	const Vec2& Position::currentPos() const
	{
		return currentPos_;
	}

	Position::operator const Vec2& () const
	{
		return currentPos();
	}

	void Position::setProposedPos(const Vec2& newPos)
	{
		proposedPos_ = newPos;
	}

	void Position::applyMoveX(double deltaX)
	{
		proposedPos_.x = currentPos_.x + deltaX;
	}

	void Position::applyMoveY(double deltaY)
	{
		proposedPos_.y = currentPos_.y + deltaY;
	}

	void Position::revertMoveX()
	{
		proposedPos_.x = currentPos_.x;
	}

	void Position::revertMoveY()
	{
		proposedPos_.y = currentPos_.y;
	}

	void Position::confirm()
	{
		currentPos_ = proposedPos_;
	}
}

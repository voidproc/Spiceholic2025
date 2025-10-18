#pragma once

namespace Spiceholic
{
	// キャラクターの位置
	class Position
	{
	public:
		[[nodiscard]]
		Position();

		[[nodiscard]]
		Position(const Vec2& pos);

		const Vec2& proposedPos() const;

		const Vec2& currentPos() const;

		operator const Vec2& () const;

		void setProposedPos(const Vec2& newPos);

		void applyMoveX(double deltaX);

		void applyMoveY(double deltaY);

		void revertMoveX();

		void revertMoveY();

		void confirm();

	private:
		Vec2 currentPos_;
		Vec2 proposedPos_;
	};
}

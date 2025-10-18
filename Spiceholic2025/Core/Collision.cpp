#include "Collision.h"

namespace Spiceholic
{
	Collision::Collision()
		:
		collision_{ RectF{} },
		enabled_{ false }
	{
	}

	bool Collision::isCollidingWith(const Vec2& thisPos, const Collision& other, const Vec2& otherPos) const
	{
		if (not enabled() || not other.enabled()) return false;

		return std::visit([&](auto&& thisShape) -> bool {
			return std::visit([&](auto&& otherShape) -> bool {
				return thisShape.movedBy(thisPos).intersects(otherShape.movedBy(otherPos));
			}, other.collision_);
		}, collision_);
	}

	bool Collision::enabled() const noexcept
	{
		return enabled_;
	}

	void Collision::enable(bool value) noexcept
	{
		enabled_ = value;
	}

	void Collision::disable() noexcept
	{
		enable(false);
	}

	void Collision::draw(const Vec2& pos) const
	{
		std::visit([&](auto&& shape) {
			shape.movedBy(pos).drawFrame(1, Palette::Lime);
		}, collision_);
	}
}

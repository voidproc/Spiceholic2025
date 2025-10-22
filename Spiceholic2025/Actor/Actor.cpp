#include "Actor.h"

namespace Spiceholic
{
	Actor::Actor(const Vec2& pos)
		:
		pos_{ pos },
		moveAmount_{},
		active_{ true },
		life_{ 1.0 },
		timerDamaged_{ 0.3s, StartImmediately::No, Clock() }
	{
	}

	void Actor::update()
	{
	}

	void Actor::draw() const
	{
	}

	void Actor::onCollide(Actor* /*actor*/)
	{
	}

	void Actor::onDead()
	{
	}

	const Position& Actor::position() const
	{
		return pos_;
	}

	void Actor::setCurrentPosition(const Vec2& newPos)
	{
		pos_.setProposedPos(newPos);
		pos_.confirm();
	}

	void Actor::setMoveAmount(const Vec2& moveAmount, bool apply)
	{
		moveAmount_ = moveAmount;

		if (apply)
		{
			applyMoveX();
			applyMoveY();
		}
	}

	const Vec2& Actor::getMoveAmount() const
	{
		return moveAmount_;
	}

	void Actor::applyMoveX(double ratio)
	{
		pos_.applyMoveX(moveAmount_.x * ratio);
	}

	void Actor::applyMoveY(double ratio)
	{
		pos_.applyMoveY(moveAmount_.y * ratio);
	}

	void Actor::revertMoveX()
	{
		pos_.revertMoveX();
	}

	void Actor::revertMoveY()
	{
		pos_.revertMoveY();
	}

	void Actor::confirmPosition()
	{
		pos_.confirm();
	}
	bool Actor::active() const
	{
		return active_;
	}

	void Actor::setInactive(bool callOnDead)
	{
		active_ = false;

		if (callOnDead)
		{
			onDead();
		}
	}

	double Actor::life() const
	{
		return life_;
	}

	void Actor::setLife(double value)
	{
		life_ = value;
	}

	void Actor::setDamage(double value)
	{
		if (not timerDamaged_.isRunning() && not invincible())
		{
			life_ -= value;

			timerDamaged_.restart();

			if (life_ <= 0)
			{
				setInactive();
			}
		}
	}

	bool IsCollidable(const Actor& actor1, const Actor& actor2)
	{
		auto tag1 = actor1.tag();
		auto tag2 = actor2.tag();

		const auto check = [](auto tagA, auto tagB) {
			return
				((tagA == ActorTag::Player) && (tagB == ActorTag::Enemy)) ||
				((tagA == ActorTag::Player) && (tagB == ActorTag::Item)) ||
				((tagA == ActorTag::Weapon) && (tagB == ActorTag::Enemy)) ||
				((tagA == ActorTag::Weapon) && (tagB == ActorTag::Block))
				;
			};

		return check(tag1, tag2) || check(tag2, tag1);
	}
}

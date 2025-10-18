#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Player : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Player;
		inline static constexpr ActorType Type = ActorType::PlayerA;

	public:
		Player(const Vec2& pos);

		~Player() override;

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		Collision collision_;
	};
}

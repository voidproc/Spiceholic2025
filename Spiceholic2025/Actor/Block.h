#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Block : public Actor
	{
	public:
		static constexpr ActorTag Tag = ActorTag::Block;

	public:
		Block(const Vec2& pos, ActorType type, GameData& gameData, bool hasKey = false, bool secretRoute = false);

		~Block() override;

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		void onDead() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return type_; }

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

		bool invincible() const override;

		void explode();

		void setInactiveIfSecret();

	private:
		ActorType type_;
		GameData& gameData_;
		bool hasKey_;
		bool secretRoute_;
		Collision collision_;
	};
}

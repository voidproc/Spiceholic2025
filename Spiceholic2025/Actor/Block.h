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
		Block(const Vec2& pos, ActorType type);

		~Block() override;

		void update() override;

		void draw() const override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return type_; }

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		ActorType type_;
		Collision collision_;
	};
}

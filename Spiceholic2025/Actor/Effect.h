#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Fx : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Effect;
		inline static constexpr ActorType Type = ActorType::None;

	public:
		Fx(const Vec2& pos);

		~Fx() override;

		void update() override;

		void draw() const override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		Collision collision_;
	};

	class FxBlockBreak : public Fx
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Effect;
		inline static constexpr ActorType Type = ActorType::None;

	public:
		FxBlockBreak(const Vec2& pos);

		~FxBlockBreak() override;

		void update() override;

		void draw() const override;

	private:
		Timer timer_;
		Timer timer2_;
	};

	class FxTwinkle : public Fx
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Effect;
		inline static constexpr ActorType Type = ActorType::None;

	public:
		FxTwinkle(const Vec2& pos);

		~FxTwinkle() override;

		void update() override;

		void draw() const override;

	private:
		Timer timer_;
		double angle_;
	};

}

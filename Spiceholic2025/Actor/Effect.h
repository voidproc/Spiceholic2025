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
		FxBlockBreak(const Vec2& pos, bool secret = false);

		~FxBlockBreak() override;

		void update() override;

		void draw() const override;

	private:
		Timer timer_;
		Timer timer2_;
		bool secret_;
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

	class FxSmoke : public Fx
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Effect;
		inline static constexpr ActorType Type = ActorType::None;

	public:
		FxSmoke(const Vec2& pos, const Actor* target, double scale, double vel = 1.0, double delay = 0);

		~FxSmoke() override;

		void update() override;

		void draw() const override;

	private:
		const Actor* target_;
		const double scale_;
		const double vel_;//12に対する倍率
		Timer timerDelay_;
		const Vec2 offset_;
		Timer timer_;
		double angle_;
	};

	class FxSmoke2 : public Fx
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Effect;
		inline static constexpr ActorType Type = ActorType::None;

	public:
		FxSmoke2(GameData& gameData, const Vec2& pos, double scale, double vel = 1.0, double delay = 0);

		~FxSmoke2() override;

		void update() override;

		void draw() const override;

	private:
		GameData& gameData_;
		double scale_;
		double vel_;
		bool mirror_;
		Timer timerDelay_;
		Stopwatch time_;

	};
}

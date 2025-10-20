#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Enemy : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Enemy;

	public:
		Enemy(const Vec2& pos, GameData& gameData);

		~Enemy() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return ActorType::None; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		void onDead() override;

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

		bool invincible() const override;

	protected:
		Collision collision_;
		GameData& gameData_;
		bool spriteMirror_;
		Vec2 drawOffset_;
	};

	class EnemyChick : public Enemy
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Enemy;
		inline static constexpr ActorType Type = ActorType::EnemyChick;

	public:
		EnemyChick(const Vec2& pos, GameData& gameData);

		~EnemyChick() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

	private:
		Vec2 dir_;
		Vec2 currentSpeed_;
	};

}

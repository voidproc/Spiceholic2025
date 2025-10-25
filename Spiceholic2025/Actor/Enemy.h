#pragma once

#include "Actor.h"
#include "Direction.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Enemy : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Enemy;

	public:
		Enemy(const Vec2& pos, GameData& gameData, const String& bringItems);

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
		void onGetKey_();

		void explode_();

		Collision collision_;
		GameData& gameData_;
		String bringItems_;
		bool spriteMirror_;
		Vec2 drawOffset_;
	};

	class EnemyChick : public Enemy
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Enemy;
		inline static constexpr ActorType Type = ActorType::EnemyChick;

	public:
		EnemyChick(const Vec2& pos, GameData& gameData, Direction dir, int32 subType, const String& bringItems);

		~EnemyChick() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

	private:
		int32 subType_;
		Direction dirType_;
		Vec2 dir_;
		Vec2 currentSpeed_;
	};

}

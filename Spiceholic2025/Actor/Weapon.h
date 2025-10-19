#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Weapon : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Weapon;

	public:
		Weapon(const Vec2& pos);

		~Weapon() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return ActorType::None; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		Collision collision_;
	};

	// プレイヤーが吐く炎
	// 何かに当たると無効になり、爆発する→WeaponExplode
	// 無効になるまで、power 個を時間差で吐いた方向に生成していく
	class WeaponFire : public Weapon
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Weapon;
		inline static constexpr ActorType Type = ActorType::WeaponFire;

	public:
		WeaponFire(const Vec2& pos, const Vec2& dir, int32 power, GameData& gameData);

		~WeaponFire() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		Collision collision_;
		Vec2 dir_;
		int32 power_;
		GameData& gameData_;
		Stopwatch time_;

		// Fire
		bool makeNextFire_;
	};

	// 爆発
	// 攻撃判定がある
	class WeaponExplode : public Weapon
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Weapon;
		inline static constexpr ActorType Type = ActorType::WeaponExplode;

	public:
		WeaponExplode(const Vec2& pos, double scale, double delay);

		~WeaponExplode() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return Type; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

	private:
		double scale_;
		Timer delay_;
		Collision collision_;
		Stopwatch time_;
	};
}

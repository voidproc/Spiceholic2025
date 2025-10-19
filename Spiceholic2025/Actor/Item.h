#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	class Item : public Actor
	{
	public:
		inline static constexpr ActorTag Tag = ActorTag::Item;

	public:
		Item(const Vec2& pos, ActorType itemType, GameData& gameData);

		~Item() override;

		ActorTag tag() const override { return Tag; }
		ActorType type() const override { return type_; }

		void update() override;

		void draw() const override;

		void onCollide(Actor* other) override;

		// ICollidable
		const Vec2& getCollisionPos() const override;

		// ICollidable
		const Collision& getCollision() const override;

		int32 score() const;

	private:
		GameData& gameData_;
		Collision collision_;
		ActorType type_;

		// キラキラエフェクト生成
		Timer timerMakeFx_;
	};
}

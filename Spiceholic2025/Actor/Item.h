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
		Item(const Vec2& pos, ActorType itemType, GameData& gameData, bool appearJumping = false, double delay = 0);

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

		int32 score() const override;

	private:
		void initCollision_();

		GameData& gameData_;
		Collision collision_;
		ActorType type_;
		Stopwatch time_;
		bool appearJumping_;
		Timer timerDelay_;

		// 跳ねながら出現用
		Timer timerJumping_;
		double jumpHeight_;

		// プレイヤーに向かう
		Stopwatch timeJumpToPlayer_;

		// キラキラエフェクト生成
		Timer timerMakeFx_;
	};
}

#pragma once

#include "Actor.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	struct GameData;

	class Player : public Actor
	{
	public:
		static constexpr ActorTag Tag = ActorTag::Player;
		static constexpr ActorType Type = ActorType::PlayerA;

	public:
		Player(const Vec2& pos, GameData& gameData);

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

		void updateSpriteState_();

	private:
		GameData& gameData_;
		Collision collision_;
		String moveDirection_;
		String spriteName_;
		bool spriteMirror_;
	};
}

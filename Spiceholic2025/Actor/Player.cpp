#include "Player.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"

namespace Spiceholic
{
	namespace
	{
		constexpr double DefaultPlayerMoveSpeed = 1.2 * 60;
	}

	Player::Player(const Vec2& pos)
		:
		Actor{ pos },
		collision_{}
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 16 - 2 });
	}

	Player::~Player()
	{
	}

	void Player::update()
	{
		const double MoveSpeed = DefaultPlayerMoveSpeed;

		const Vec2 playerMoveAmount{
			(KeyRight.pressed() - KeyLeft.pressed()) * MoveSpeed,
			(KeyDown.pressed() - KeyUp.pressed()) * MoveSpeed
		};

		setMoveAmount(playerMoveAmount.limitLength(MoveSpeed) * Scene::DeltaTime());
	}

	void Player::draw() const
	{
		if (getMoveAmount().length() > 1e-6)
		{
			// 歩きモーション
			const int charaAnimFrame = PeriodicStair(0.8s, 0, 3);
			TextureAsset(U"Chara")(charaAnimFrame * 48, 0, 48).drawAt(position());
		}
		else
		{
			// 立ちモーション
			const int charaAnimFrame = PeriodicStair(0.9s, 4, 5);
			TextureAsset(U"Chara")(charaAnimFrame * 48, 0, 48).drawAt(position());
		}
	}

	void Player::onCollide(Actor* other)
	{
		// TODO: 敵に触れると...
		if (other->tag() == ActorTag::Enemy)
		{
			//...
		}
	}

	const Vec2& Player::getCollisionPos() const
	{
		return position().proposedPos();
	}

	const Collision& Player::getCollision() const
	{
		return collision_;
	}

}

#include "Player.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		constexpr double DefaultPlayerMoveSpeed = 1.2 * 60;
	}

	Player::Player(const Vec2& pos, GameData& gameData)
		:
		Actor{ pos },
		gameData_{ gameData },
		collision_{},
		moveDirection_{ U"" },
		spriteName_{ U"PlayerStand" },
		spriteMirror_{ false }
	{
		collision_.set(RectF{ Arg::center = Vec2{}, SizeF{ 16 - 2, 20 - 2 } });
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

		// 移動方向によりスプライト名を決定
		updateSpriteState_();
	}

	void Player::draw() const
	{
		const SpriteInfo& sprite = gameData_.appSetting->get().sprite[spriteName_];

		const int animFrame = PeriodicStair(0.8s, 0, sprite.count - 1);
		TextureAsset(sprite.textureName)(sprite.pos + Vec2{ animFrame * sprite.size, 0 }, sprite.size, sprite.size)
			.mirrored(spriteMirror_)
			.drawAt(position());
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

	void Player::updateSpriteState_()
	{
		if (getMoveAmount().length() < 1e-6)
		{
			spriteName_ = U"PlayerStand{}"_fmt(moveDirection_);
			return;
		}

		const auto angle = getMoveAmount().getAngle();
		if (InRange(Abs(angle), 45_deg, 135_deg))
		{
			moveDirection_ = U"R";
			spriteMirror_ = (angle < 0);
		}
		else if (Abs(angle) > 45_deg)
		{
			moveDirection_ = U"";
		}
		else if (Abs(angle) < 45_deg)
		{
			moveDirection_ = U"U";
		}

		spriteName_ = U"PlayerWalk{}"_fmt(moveDirection_);
	}
}

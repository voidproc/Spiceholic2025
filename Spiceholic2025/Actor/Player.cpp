#include "Player.h"
#include "Weapon.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"
#include "Input/ActionInput.h"
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
		timerFire_{ 1s, StartImmediately::No, Clock() },
		timerGetItem_{ 0.3s, StartImmediately::No, Clock() },
		timerKnockback_{ 0.4s, StartImmediately::No, Clock() },
		timerTr_{ 1.8s, StartImmediately::No, Clock() },
		moveDirection_{ Direction::S },
		moveDirectionText_{ U"" },
		spriteName_{ U"PlayerStand" },
		spriteMirror_{ false },
		drawOffset_{}
	{
		collision_.set(RectF{ Arg::center = Vec2{ 0, 4 }, SizeF{ 16 - 2, 16 - 2 } });
	}

	Player::~Player()
	{
	}

	void Player::update()
	{
		if (not timerFire_.isRunning() && not timerKnockback_.isRunning())
		{
			// 硬直していない、かつノックバック中じゃない

			// 移動
			{
				const double MoveSpeed = DefaultPlayerMoveSpeed;

				const Vec2 playerMoveAmount{
					(gameData_.actionInput->pressed(Action::MoveRight) - gameData_.actionInput->pressed(Action::MoveLeft)) * MoveSpeed,
					(gameData_.actionInput->pressed(Action::MoveDown) - gameData_.actionInput->pressed(Action::MoveUp)) * MoveSpeed
				};

				setMoveAmount(playerMoveAmount.limitLength(MoveSpeed) * Scene::DeltaTime());
			}

			// 炎を吐く
			if (gameData_.actionInput->pressed(Action::Attack))
			{
				const Circular fireDir{ 14, DirectionToAngle(moveDirection_) };
				gameData_.actors.push_back(std::make_unique<WeaponFire>(position().currentPos() + Vec2{ 0, 0 } + fireDir, fireDir, 3, gameData_));

				// 炎を吐いたタイマー: 動作中、プレイヤーは硬直する
				timerFire_.restart(0.5s);
			}

			drawOffset_ = Vec2::Zero();

			// 移動方向によりスプライト名を決定
			updateSpriteState_();
		}
		else if (timerFire_.isRunning())
		{
			// 硬直中

			// 少し後退
			setMoveAmount(Circular{ 12.0, DirectionToAngle(moveDirection_) + 180_deg }.toVec2() * Scene::DeltaTime());

			drawOffset_.x = Periodic::Sine1_1(0.08s) * 1.5;
		}
		else if (timerKnockback_.isRunning())
		{
			// ノックバック中

			// 少し後退
			setMoveAmount(Circular{ 60.0, DirectionToAngle(moveDirection_) + 180_deg }.toVec2() * Scene::DeltaTime());
		}

	}

	void Player::draw() const
	{
		const SpriteInfo& sprite = gameData_.appSetting->get().sprite[spriteName_];
		const int animFrame = PeriodicStair(0.8s, 0, sprite.count - 1, ClockTime());

		// ノックバック中の描画オフセット
		const Vec2 knockbackOffset = timerKnockback_.isRunning() ? Vec2{ 0, -8 * Periodic::Jump0_1(timerKnockback_.duration(), timerKnockback_.progress0_1() * timerKnockback_.duration().count()) } : Vec2{};

		// 描画位置
		const Vec2 pos = position().currentPos() + drawOffset_ + knockbackOffset;

		// アルファ（無敵中は点滅）
		const double alpha = (timerTr_.isRunning()) ? 0.2 + 0.7 * Periodic::Square0_1(0.08s, ClockTime()) : 1.0;

		{
			// アイテム取得時点滅
			const double t = (timerGetItem_.isRunning()) ? Periodic::Square0_1(0.06s, ClockTime()) : 1;
			ScopedColorMul2D mul{ t, t, t, 1 };
			ScopedColorAdd2D add{ (1 - t) * 0.8, (1 - t) * 0.8, (1 - t) * 0, 0 };

			TextureAsset(sprite.textureName)(sprite.pos + Vec2{ animFrame * sprite.size, 0 }, sprite.size, sprite.size)
				.mirrored(spriteMirror_)
				.drawAt(pos, AlphaF(alpha));
		}
	}

	void Player::onCollide(Actor* other)
	{
		// TODO: 敵に触れると...
		if (other->tag() == ActorTag::Enemy && not timerTr_.isRunning())
		{
			// ノックバック & 無敵
			if (not timerKnockback_.isRunning())
			{
				timerKnockback_.restart();
				timerTr_.restart();
			}
		}
		else if (other->tag() == ActorTag::Item)
		{
			timerGetItem_.restart();

			if (other->type() == ActorType::ItemChilipepper)
			{
				gameData_.gauge = Saturate(gameData_.gauge + 0.15);
			}
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
			spriteName_ = U"PlayerStand{}"_fmt(moveDirectionText_);
			return;
		}

		const auto angle = getMoveAmount().getAngle();
		if (InRange(Abs(angle), 45_deg, 135_deg))
		{
			moveDirectionText_ = U"R";
			spriteMirror_ = (angle < 0);
			moveDirection_ = spriteMirror_ ? Direction::W : Direction::E;
		}
		else if (Abs(angle) > 45_deg)
		{
			moveDirectionText_ = U"";
			moveDirection_ = Direction::S;
		}
		else if (Abs(angle) < 45_deg)
		{
			moveDirectionText_ = U"U";
			moveDirection_ = Direction::N;
		}

		spriteName_ = U"PlayerWalk{}"_fmt(moveDirectionText_);
	}
}

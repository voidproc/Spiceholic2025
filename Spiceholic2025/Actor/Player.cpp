#include "Player.h"
#include "Effect.h"
#include "Weapon.h"
#include "Config/GameConfig.h"
#include "Core/Gauge.h"
#include "Core/Periodic.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"
#include "Input/ActionInput.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		constexpr double DefaultMoveSpeed = 0.9 * 60;
		constexpr double PoweredupMoveAccel = 0.3 * 60;
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
		timerPowerupSmoke_{ 0.55s, StartImmediately::Yes, Clock() },
		moveDirection_{ Direction::Down },
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
				const double MoveSpeed = DefaultMoveSpeed + (gameData_.gauge->isPoweredUp() ? PoweredupMoveAccel : 0);

				const Vec2 playerMoveAmount{
					(gameData_.actionInput->pressed(Action::MoveRight) - gameData_.actionInput->pressed(Action::MoveLeft)) * MoveSpeed,
					(gameData_.actionInput->pressed(Action::MoveDown) - gameData_.actionInput->pressed(Action::MoveUp)) * MoveSpeed
				};

				setMoveAmount(playerMoveAmount.limitLength(MoveSpeed) * Scene::DeltaTime());
			}

			// 炎を吐く
			if (gameData_.actionInput->pressed(Action::Attack))
			{
				// ゲージ量に応じて射程変化
				// ゲージ半分以上ならMAX射程
				const int nFire = gameData_.gauge->isPoweredUp() ? 7 : 3;

				const Circular fireDir{ 14, DirectionToAngle(moveDirection_) };
				gameData_.actors.push_back(std::make_unique<WeaponFire>(position().currentPos() + Vec2{ 0, 0 } + fireDir.fastToVec2() * 0.8, fireDir, nFire, gameData_));

				// 炎を吐いたタイマー: 動作中、プレイヤーは硬直する
				timerFire_.restart(0.5s);

				// すこしゲージ消費
				gameData_.gauge->add(-0.02);
			}

			drawOffset_ = Vec2::Zero();

			// 移動方向によりスプライト名を決定
			updateSpriteState_();

			// パワーアップ中の煙
			if (timerPowerupSmoke_.reachedZero())
			{
				timerPowerupSmoke_.restart();

				if (gameData_.gauge->isPoweredUp())
				{
					const Vec2 smokeOffset{ 6.0 * ((moveDirection_ == Direction::Left) ? -1 : ((moveDirection_ == Direction::Right) ? 1 : 0)), -6.0 };
					gameData_.actors.push_back(std::make_unique<FxSmoke>(position().currentPos() + smokeOffset + RandomVec2(2.0), this, 1.0));
					gameData_.actors.push_back(std::make_unique<FxSmoke>(position().currentPos() + smokeOffset + Vec2{ Random(-5.0, 5.0), Random(1.0, 2.0) }, this, 0.5));
				}
			}
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
			setMoveAmount(Circular{ 40.0, DirectionToAngle(moveDirection_) + 180_deg }.toVec2() * Scene::DeltaTime());
		}

	}

	void Player::draw() const
	{
		const SpriteInfo& sprite = gameData_.appSetting->get().sprite[spriteName_];
		const Duration animSpeed = gameData_.gauge->isPoweredUp() ? 0.5s : 0.8s;
		const int animFrame = PeriodicStair(animSpeed, 0, sprite.count - 1, ClockTime());

		// ノックバック中の描画オフセット
		const Vec2 knockbackOffset = timerKnockback_.isRunning() ? Vec2{ 0, -8 * Periodic::Jump0_1(timerKnockback_.duration(), timerKnockback_.progress0_1() * timerKnockback_.duration().count()) } : Vec2{};

		// 描画位置
		const Vec2 pos = position().currentPos() + drawOffset_ + knockbackOffset;

		// アルファ（無敵中は点滅）
		const double alpha = (timerTr_.isRunning()) ? 0.2 + 0.7 * Periodic::Square0_1(0.08s, ClockTime()) : 1.0;

		// ベースカラー: パワーアップ中に変化
		const ColorF color = gameData_.gauge->isPoweredUp() ? Palette::White.lerp(Palette::Orange, 1.0 * Periodic::Jump0_1(0.4s, ClockTime())) : Palette::White;

		{
			// アイテム取得時点滅
			const double t = (timerGetItem_.isRunning()) ? Periodic::Square0_1(0.06s, ClockTime()) : 1;
			ScopedColorMul2D mul{ t, t, t, 1 };
			ScopedColorAdd2D add{ (1 - t) * 0.8, (1 - t) * 0.8, (1 - t) * 0, 0 };

			TextureAsset(sprite.textureName)(sprite.pos + Vec2{ animFrame * sprite.size, 0 }, sprite.size, sprite.size)
				.mirrored(spriteMirror_)
				.drawAt(pos, ColorF{ color, alpha });
		}
	}

	void Player::onCollide(Actor* other)
	{
		// TODO: 敵に触れると...
		if (other->tag() == ActorTag::Enemy && not timerTr_.isRunning())
		{
			// ノックバック、無敵、ゲージ消費
			if (not timerKnockback_.isRunning())
			{
				timerKnockback_.restart();
				timerTr_.restart();

				// ゲージ消費
				gameData_.gauge->add(-0.20);
			}
		}
		else if (other->tag() == ActorTag::Item)
		{
			timerGetItem_.restart();

			if (other->type() == ActorType::ItemChilipepper)
			{
				// ゲージ回復
				// ゲージMAXイベント発行
				if (const double gaugeVal = gameData_.gauge->add(0.15);
					Abs(gaugeVal - 1.0) < 1e-3)
				{
					GetDispatch().publish(GaugeMaxEvent{});
				}
			}
			else if (other->type() == ActorType::ItemKey)
			{
				// 鍵をとったので次のステージに向かう

				// 鍵をとったイベント発行
				GetDispatch().publish(GetKeyEvent{});

				// TOOD: 敵全滅
				//...

				// 
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
			moveDirection_ = spriteMirror_ ? Direction::Left : Direction::Right;
		}
		else if (Abs(angle) > 45_deg)
		{
			moveDirectionText_ = U"";
			moveDirection_ = Direction::Down;
		}
		else if (Abs(angle) < 45_deg)
		{
			moveDirectionText_ = U"U";
			moveDirection_ = Direction::Up;
		}

		spriteName_ = U"PlayerWalk{}"_fmt(moveDirectionText_);
	}
}

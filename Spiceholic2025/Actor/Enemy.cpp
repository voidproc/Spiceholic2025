#include "Enemy.h"
#include "Effect.h"
#include "Item.h"
#include "Player.h"
#include "Block.h"
#include "MakeActor.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/Camera.h"
#include "Core/DrawSprite.h"
#include "Core/Periodic.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"
#include "Setting/AppSetting.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
	namespace
	{
		bool IntersectsAnyBlock(const Actor& actor, const Vec2& pos, const Array<std::unique_ptr<Block>>& blocks)
		{
			for (const auto& block : blocks)
			{
				if (actor.type() == ActorType::EnemyChickFly && block->type() == ActorType::BlockSpike) continue;

				if (actor.getCollision().isCollidingWith(pos, block->getCollision(), block->position()))
				{
					return true;
				}
			}
			return false;
		}
	}

	Enemy::Enemy(const Vec2& pos, GameData& gameData, const String& bringItems)
		:
		Actor{ pos },
		gameData_{ gameData },
		bringItems_{ bringItems },
		collision_{},
		spriteMirror_{ false },
		drawOffset_{}
	{
		GetDispatch().subscribe<GetKeyEvent, &Enemy::onGetKey_>(this);
	}

	Enemy::~Enemy()
	{
		GetDispatch().unsubscribe<GetKeyEvent>(this);
	}

	void Enemy::update()
	{
		if (timerDamagedSE_.isRunning())
		{
			if (not AudioAsset(U"Damage1Loop").isPlaying())
			{
				AudioAsset(U"Damage1Loop").play();
			}
		}
		else
		{
			AudioAsset(U"Damage1Loop").stop();
		}
	}

	void Enemy::draw() const
	{
	}

	void Enemy::onCollide(Actor* /*other*/)
	{
	}

	void Enemy::onDead()
	{
		// 爆発
		explode_();

		// アイテム放出
		MakeItems(bringItems_, gameData_, position().currentPos());

		// SE
		PlayAudioOneShot(U"Explosion2");
	}

	const Vec2& Enemy::getCollisionPos() const
	{
		return position();
	}

	const Collision& Enemy::getCollision() const
	{
		return collision_;
	}

	bool Enemy::invincible() const
	{
		return false;
	}

	void Enemy::onGetKey_()
	{
		setInactive(false);

		// 爆発
		explode_();

		// SE（画面内にいる場合
		if (CameraRect(gameData_).intersects(position()))
		{
			PlayAudioOneShot(U"Explosion2");
		}
	}

	void Enemy::explode_()
	{
		MakeSmokes(gameData_, position());

		gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos()));
	}

	EnemyChick::EnemyChick(const Vec2& pos, GameData& gameData, Direction dir, int32 subType, const String& bringItems, bool fly)
		:
		Enemy{ pos, gameData, bringItems },
		subType_{ subType },
		dirType_{ dir },
		dir_{},
		currentSpeed_{ 0, 0 },
		fly_{ fly }
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 12 });

		if (subType == 0)
		{
			dir_ = (dir == Direction::Left) ? Vec2::Left() : Vec2::Right();
			spriteMirror_ = (dir_.x < 0);

		}
	}

	EnemyChick::~EnemyChick()
	{
	}

	ActorType EnemyChick::type() const
	{
		return (fly_) ? ActorType::EnemyChickFly : ActorType::EnemyChick;
	}

	void EnemyChick::update()
	{
		double speed = 0;
		double speedLimit = 16.0;

		if (subType_ == 0)
		{
			// 0: ひたすら直進

			// 進行方向上のちょっと先にブロックがあったら振り向く
			if (dirType_ != Direction::None)
			{
				const Vec2 nextPos = position().currentPos() + dir_ * 8.0;
				if (IntersectsAnyBlock(*this, nextPos, gameData_.blocks))
				{
					dir_.x = -dir_.x;
					currentSpeed_ *= 0.7;
				}
			}

			spriteMirror_ = (dir_.x < 0);

			speed = 24;
			speedLimit = 16 + (8 * fly_);
		}
		else if (subType_ == 1)
		{
			// 1: プレイヤーの方を向く
			//const bool oldMirror = spriteMirror_;

			spriteMirror_ = (gameData_.player->position().currentPos().x < position().currentPos().x);
			dir_.x = spriteMirror_ ? -1 : 1;

			// 方向転換したら減速
			//if (oldMirror != spriteMirror_)
			//{
			//	currentSpeed_ /= 2;
			//}

			speed = 36;
			speedLimit = 28;
		}

		if (dirType_ != Direction::None)
		{
			currentSpeed_ = currentSpeed_ + (dir_ * speed * Scene::DeltaTime());
			currentSpeed_.x = Clamp(currentSpeed_.x, -speedLimit, speedLimit);
			setMoveAmount(currentSpeed_ * Scene::DeltaTime());
		}
	}

	void EnemyChick::draw() const
	{
		{
			// ダメージ点滅
			const double t = (timerDamaged_.isRunning()) ? Periodic::Square0_1(0.06s, ClockTime()) : 1;
			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1 - t, 0 };

			DrawSprite(*gameData_.appSetting, (fly_) ? U"EnemyChickFly" : U"EnemyChick", 0.65s, spriteMirror_, position().currentPos() + drawOffset_);
		}
	}

	void EnemyChick::onCollide(Actor* /*other*/)
	{

	}
}

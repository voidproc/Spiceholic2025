#include "Enemy.h"
#include "Effect.h"
#include "Item.h"
#include "Player.h"
#include "Block.h"
#include "MakeActor.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
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

		// SE
		PlayAudioOneShot(U"Explosion2");
	}

	void Enemy::explode_()
	{
		MakeSmokeFxN(gameData_.actors, position().currentPos(), 3, { 6, 12 }, { -8, 0 }, { 0.9, 1.5 }, 0.3, 0.08);

		gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos()));
	}

	EnemyChick::EnemyChick(const Vec2& pos, GameData& gameData, Direction dir, int32 subType, const String& bringItems)
		:
		Enemy{ pos, gameData, bringItems },
		subType_{ subType },
		dirType_{ dir },
		dir_{},
		currentSpeed_{ 0, 0 }
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

	void EnemyChick::update()
	{
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
				}
			}

			spriteMirror_ = (dir_.x < 0);
		}
		else if (subType_ == 1)
		{
			// 1: プレイヤーの方を向く
			spriteMirror_ = (gameData_.player->position().currentPos().x < position().currentPos().x);
			dir_.x = spriteMirror_ ? -1 : 1;
		}

		if (dirType_ != Direction::None)
		{
			currentSpeed_ = currentSpeed_ + (dir_ * 24 * Scene::DeltaTime());
			currentSpeed_.x = Clamp(currentSpeed_.x, -16.0, 16.0);
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

			DrawSprite(*gameData_.appSetting, U"EnemyChick", 0.65s, spriteMirror_, position().currentPos() + drawOffset_);
		}
	}

	void EnemyChick::onCollide(Actor* /*other*/)
	{

	}
}

#include "Enemy.h"
#include "Effect.h"
#include "Item.h"
#include "Player.h"
#include "Block.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"
#include "Setting/AppSetting.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
	namespace
	{
		void DrawSprite(const AppSetting& appSetting, StringView spriteName, const Duration& animSpeed, bool mirror, const Vec2& pos)
		{
			const SpriteInfo& sprite = appSetting.get().sprite.at(spriteName);

			const int animFrame = PeriodicStair(animSpeed, 0, sprite.count - 1, ClockTime());
			TextureAsset(sprite.textureName)(sprite.pos + Vec2{ animFrame * sprite.size, 0 }, sprite.size, sprite.size)
				.mirrored(mirror)
				.drawAt(pos);
		}

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

	Enemy::Enemy(const Vec2& pos, GameData& gameData)
		:
		Actor{ pos },
		gameData_{ gameData },
		collision_{},
		spriteMirror_{ false },
		drawOffset_{}
	{
	}

	Enemy::~Enemy()
	{
	}

	void Enemy::update()
	{
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
		gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos()));

		// アイテム放出
		// TODO: 敵によって放出量を変える
		const double a = Random(Math::TwoPi);
		for (int i = 0, n = 3; i < n; ++i)
		{
			const Vec2 pos = position().currentPos() + Circular{ 12, a + 120_deg * i };
			gameData_.actors.push_back(std::make_unique<Item>(pos, ActorType::ItemChilipepper, gameData_, true, 0.04 * i));
		}
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

	EnemyChick::EnemyChick(const Vec2& pos, GameData& gameData, Direction dir, int32 subType)
		:
		Enemy{ pos, gameData },
		subType_{ subType },
		dir_{},
		currentSpeed_{ 0, 0 }
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 12 });

		if (subType == 0)
		{
			dir_ = (dir == Direction::Left) ? Vec2::Left() : Vec2::Right();
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
			const Vec2 nextPos = position().currentPos() + dir_ * 8.0;
			if (IntersectsAnyBlock(*this, nextPos, gameData_.blocks))
			{
				dir_.x = -dir_.x;
			}

			spriteMirror_ = (dir_.x < 0);
		}
		else if (subType_ == 1)
		{
			// 1: プレイヤーの方を向く
			spriteMirror_ = (gameData_.player->position().currentPos().x < position().currentPos().x);
			dir_.x = spriteMirror_ ? -1 : 1;
		}

		currentSpeed_ = currentSpeed_ + (dir_ * 24 * Scene::DeltaTime());
		currentSpeed_.x = Clamp(currentSpeed_.x, -16.0, 16.0);

		setMoveAmount(currentSpeed_ * Scene::DeltaTime());
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

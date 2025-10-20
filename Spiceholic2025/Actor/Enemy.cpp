#include "Enemy.h"
#include "Effect.h"
#include "Item.h"
#include "Player.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"
#include "Setting/AppSetting.h"

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
		for (int i = 0, n = Random(3, 3), a = Random(Math::TwoPi); i < n; ++i)
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

	EnemyChick::EnemyChick(const Vec2& pos, GameData& gameData)
		:
		Enemy{ pos, gameData },
		dir_{ -1, 0 },
		currentSpeed_{ 0, 0 }
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 12 });
	}

	EnemyChick::~EnemyChick()
	{
	}

	void EnemyChick::update()
	{
		// プレイヤーの方を向く
		spriteMirror_ = (gameData_.player->position().currentPos().x < position().currentPos().x);
		dir_.x = spriteMirror_ ? -1 : 1;

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

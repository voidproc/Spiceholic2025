#include "Enemy.h"
#include "Actor/Player.h"
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

	const Vec2& Enemy::getCollisionPos() const
	{
		return position();
	}

	const Collision& Enemy::getCollision() const
	{
		return collision_;
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

			DrawSprite(*gameData_.appSetting, U"EnemyChick", 0.8s, spriteMirror_, position().currentPos() + drawOffset_);
		}
	}

	void EnemyChick::onCollide(Actor* /*other*/)
	{

	}
}

#include "Item.h"
#include "Effect.h"
#include "Player.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
		// あたり判定の大きさ
		constexpr double DefaultItemCollisionSize = 16;

		// 登場にかかる時間
		constexpr double TimeAppearSec = 0.3;
		constexpr Duration TimeAppear{ TimeAppearSec };

	}

	Item::Item(const Vec2& pos, ActorType itemType, GameData& gameData, bool appearJumping)
		:
		Actor{ pos },
		gameData_{ gameData },
		collision_{},
		type_{ itemType },
		timerJumping_{ 999s },
		timerMakeFx_{ 1.1s, StartImmediately::Yes }
	{
		if (appearJumping)
		{
			timerJumping_.restart(TimeAppear);
		}
		else
		{
			initCollision_();
			timerJumping_.reset();
		}
	}

	Item::~Item()
	{
	}

	void Item::update()
	{
		// 跳ねながら登場の場合
		if (timerJumping_.reachedZero())
		{
			initCollision_();
			timerJumping_.reset();
		}

		// きらきら
		if (timerMakeFx_.reachedZero())
		{
			timerMakeFx_.restart(Duration{ Random(0.8, 1.3) });
			gameData_.actors.push_back(std::make_unique<FxTwinkle>(position().currentPos() + Circular{ Random(10.0, 12.0), Random(Math::TwoPi) }));
		}
	}

	void Item::draw() const
	{
		const double jumpY = timerJumping_.isRunning() ? -4.0 * Periodic::Jump0_1(TimeAppear) : 0;

		const Vec2 pos = position().currentPos() + Vec2{ 0, 1.0 * Periodic::Sine1_1(1.5s) } + Vec2{ 0, jumpY };
		const ColorF color{ 1 };

		if (type() == ActorType::ItemChilipepper)
		{
			const double t = 1.0 - EaseInOutCubic(Periodic::Triangle0_1(0.5s)) * Periodic::Square0_1(1.0s);
			ScopedColorMul2D mul(t, t, t, 1);
			ScopedColorAdd2D add(1 - t, 1 - t, 1 - t, 0);

			TextureAsset(U"Item")(0, 0, 24).drawAt(pos, color);
		}
	}

	void Item::onCollide(Actor* /*other*/)
	{
		if (type() == ActorType::ItemChilipepper)
		{
			// なにかしらの効果（イベント発行など）
			//...
		}

		// 破棄待ちの状態へ
		setInactive();
	}

	const Vec2& Item::getCollisionPos() const
	{
		return position();
	}

	const Collision& Item::getCollision() const
	{
		return collision_;
	}

	int32 Item::score() const
	{
		int32 score = 0;

		if (type_ == ActorType::ItemChilipepper)
		{
			score = 100;
		}

		return score;
	}

	void Item::initCollision_()
	{
		collision_.set(RectF{ Arg::center = Vec2{}, DefaultItemCollisionSize });
	}
}

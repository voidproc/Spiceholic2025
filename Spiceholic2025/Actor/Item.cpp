#include "Item.h"
#include "Effect.h"
#include "Player.h"
#include "Config/GameConfig.h"
#include "Core/DrawSprite.h"

namespace Spiceholic
{
	namespace
	{
		// あたり判定の大きさ
		constexpr double DefaultItemCollisionSize = 16;

		// 出現にかかる時間
		constexpr double TimeAppearSec = 0.27;
		constexpr Duration TimeAppear{ TimeAppearSec };

	}

	Item::Item(const Vec2& pos, ActorType itemType, GameData& gameData, bool appearJumping, double delay)
		:
		Actor{ pos },
		gameData_{ gameData },
		collision_{},
		type_{ itemType },
		time_{ StartImmediately::No, Clock() },
		appearJumping_{ appearJumping },
		timerDelay_{ Duration{ delay }, StartImmediately::Yes, Clock() },
		timerJumping_{ 999s, StartImmediately::No, Clock() },
		jumpHeight_{ Random(4.0, 7.0) },
		timerMakeFx_{ 1.1s, StartImmediately::No, Clock() }
	{
	}

	Item::~Item()
	{
	}

	void Item::update()
	{
		if (timerDelay_.reachedZero())
		{
			timerDelay_ = Timer{ 999s, StartImmediately::No, Clock() };
			time_.start();
			timerMakeFx_.start();

			if (appearJumping_)
			{
				timerJumping_.restart(TimeAppear * Random(0.60, 1.5));
			}
			else
			{
				initCollision_();
				timerJumping_.reset();
			}
		}

		// 跳ねながら出現の場合
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
		// 跳ねる
		const Vec2 jump{ 0, timerJumping_.isRunning() ? -jumpHeight_ * Periodic::Jump0_1(TimeAppear, timerJumping_.progress0_1() * TimeAppearSec) : 0 };

		const Vec2 pos = position().currentPos()
			+ ((type() == ActorType::ItemKey) ? Vec2{} : Vec2{ 0, 1.0 * Periodic::Sine1_1(1.5s, time_.sF()) })
			+ jump;
		const ColorF color{ 1 };

		// 白点滅
		const double t = 1.0 - EaseInOutCubic(Periodic::Triangle0_1(0.5s, time_.sF())) * Periodic::Square0_1(1.0s, time_.sF());
		ScopedColorMul2D mul(t, t, t, 1);
		ScopedColorAdd2D add(1 - t, 1 - t, 1 - t, 0);

		if (type() == ActorType::ItemChilipepper)
		{
			TextureAsset(U"Item")(0, 0, 24).drawAt(pos, color);
		}
		else if (type() == ActorType::ItemKey)
		{
			DrawSprite(*gameData_.appSetting, U"ItemKey", 1s, false, pos);
		}
	}

	void Item::onCollide(Actor* /*other*/)
	{
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
		else if (type_ == ActorType::ItemKey)
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

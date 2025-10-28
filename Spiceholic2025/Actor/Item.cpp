#include "Item.h"
#include "Effect.h"
#include "Player.h"
#include "Config/GameConfig.h"
#include "Core/DrawSprite.h"
#include "Core/Periodic.h"

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
		timerMakeFx_{ 1.1s, StartImmediately::No, Clock() },
		timeJumpToPlayer_{ StartImmediately::No, Clock() }
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

		// プレイヤーに向かっている
		if (timeJumpToPlayer_.isRunning())
		{
			const auto playerPos = gameData_.player->position().currentPos();
			const auto direction = playerPos - position().currentPos() - Vec2{ 0, 20 } * (1 - EaseOutQuad(Saturate(timeJumpToPlayer_.sF() / 0.5)));
			setMoveAmount(direction.limitLength(60.0 * 2 * Scene::DeltaTime() * (0.7 + 1.0 * EaseInSine(Saturate(timeJumpToPlayer_.sF() / 0.5)))), true);
			confirmPosition();
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
		// プレイヤーに向かい中
		if (timeJumpToPlayer_.isRunning())
		{
			const int frame = PeriodicStair(Duration{ 0.4 }, 0, 7, timeJumpToPlayer_.sF());
			TextureAsset(U"BurningLoop1")(frame * 24, 0, 24, 32).drawAt(position().currentPos() - Vec2{0,4}, ColorF{ 1, 0.7 * Periodic::Square0_1(0.07s, ClockTime()) });
		}

		// 跳ねる
		const Vec2 jump{ 0, timerJumping_.isRunning() ? -jumpHeight_ * Periodic::Jump0_1(TimeAppear, timerJumping_.progress0_1() * TimeAppearSec) : 0 };

		const Vec2 pos = position().currentPos()
			+ ((type() == ActorType::ItemKey) ? Vec2{} : Vec2{ 0, 1.0 * Periodic::Sine1_1(1.5s, time_.sF()) })
			+ jump;
		const ColorF color{ 1 };

		// 白点滅
		const double alpha = timeJumpToPlayer_.isRunning() ? 0.4 + 0.6 * Periodic::Square0_1(0.07s) : 1;
		const double t = 1.0 - EaseInOutCubic(Periodic::Triangle0_1(0.5s, time_.sF())) * Periodic::Square0_1(1.0s, time_.sF());
		ScopedColorMul2D mul(t, alpha);
		ScopedColorAdd2D add(1 - t, 0);

		if (type() == ActorType::ItemChilipepper)
		{
			TextureAsset(U"Item")(0, 0, 24).drawAt(pos, color);
		}
		else if (type() == ActorType::ItemKey)
		{
			DrawSprite(*gameData_.appSetting, U"ItemKey", 1s, false, pos);
		}
		else
		{
			Circle{ position(), 24 + 2 * Periodic::Sine0_1(0.8, time_.sF()) }.drawFrame(1.0, HSV{ time_.sF() * 360 * 2.0, 1.0, 1.0 }.withAlpha(Periodic::Square0_1(0.09s, time_.sF())));

			if (type() == ActorType::ItemCurry)
			{
				DrawSprite(*gameData_.appSetting, U"ItemCurry", 1s, false, pos);
			}
			else if (type() == ActorType::ItemHabanero)
			{
				DrawSprite(*gameData_.appSetting, U"ItemHabanero", 1s, false, pos);
			}
			else if (type() == ActorType::ItemMapo)
			{
				DrawSprite(*gameData_.appSetting, U"ItemMapo", 1s, false, pos);
			}
		}
	}

	void Item::onCollide(Actor* other)
	{
		if (other->tag() == ActorTag::Player)
		{
			// 破棄待ちの状態へ
			setInactive();
		}
		else if (other->tag() == ActorTag::Weapon &&
			not timeJumpToPlayer_.isRunning() &&
			time_ > 1s)
		{
			// プレイヤーに吸収
			timeJumpToPlayer_.start();
		}
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

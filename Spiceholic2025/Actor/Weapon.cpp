#include "Weapon.h"
#include "Actor/Player.h"
#include "Config/GameConfig.h"
#include "Core/Periodic.h"

namespace Spiceholic
{
	namespace
	{
		namespace Fire
		{
			// 1コマあたりの時間
			constexpr double TimePerFrame1 = 0.6 / 16;
			constexpr double TimePerFrame2 = 0.8 / 16;
			constexpr double TimePerFrame3 = 0.6 / 16;

			// 最初の4コマにかかる時間
			constexpr double TimeFire1 = TimePerFrame1 * 4;

			// ループ素材の1ループにかかる時間
			constexpr double TimeFire2PerLoop = TimePerFrame2 * 8;

			// ループ回数
			constexpr int Loop = 1;

			// 最後の5コマにかかる時間
			constexpr double TimeFire3 = TimePerFrame3 * 5;

			// 描画位置調整
			constexpr Vec2 DrawOffset{ 0, -6 };

			// 次の炎を生成するまでの時間
			constexpr double TimeNextFire = 0.18;
		}

		namespace Explode
		{
			constexpr double LifeTime = 0.4;
		}
	}

	Weapon::Weapon(const Vec2& pos)
		:
		Actor{ pos },
		collision_{}
	{
	}

	Weapon::~Weapon()
	{
	}

	void Weapon::update()
	{
	}

	void Weapon::draw() const
	{
	}

	void Weapon::onCollide(Actor* /*other*/)
	{
		setInactive();
	}

	const Vec2& Weapon::getCollisionPos() const
	{
		return position();
	}

	const Collision& Weapon::getCollision() const
	{
		return collision_;
	}

	WeaponFire::WeaponFire(const Vec2& pos, const Vec2& dir, int32 power, GameData& gameData)
		:
		Weapon{ pos },
		collision_{},
		dir_{ dir },
		power_{ power },
		gameData_{ gameData },
		time_{ StartImmediately::Yes, Clock() },
		makeNextFire_{ false }
	{
		collision_.set(RectF{ Arg::center = Vec2{ 0, 4 }, SizeF{ 14, 14 } });
	}

	WeaponFire::~WeaponFire()
	{
	}

	void WeaponFire::update()
	{
		// 次の炎を生成
		if (time_.sF() > Fire::TimeNextFire && not makeNextFire_ && power_ > 0)
		{
			gameData_.actors.push_back(std::make_unique<WeaponFire>(position().currentPos() + dir_, dir_, power_ - 1, gameData_));
			makeNextFire_ = true;
		}

		// 生存時間を超えた
		if (time_.sF() > Fire::TimeFire1 + Fire::TimeFire2PerLoop * Fire::Loop + Fire::TimeFire3)
		{
			setInactive();
		}
	}

	void WeaponFire::draw() const
	{
		const Vec2 drawPos = position().currentPos() + Fire::DrawOffset;

		if (time_.sF() < Fire::TimeFire1)
		{
			const int frame = PeriodicStair(Duration{ Fire::TimeFire1 }, 0, 3, time_.sF());
			TextureAsset(U"BurningStart1")(frame * 24, 0, 24, 32).drawAt(drawPos);
		}
		else if (time_.sF() < Fire::TimeFire1 + Fire::TimeFire2PerLoop * Fire::Loop)
		{
			const int frame = PeriodicStair(Duration{ Fire::TimeFire2PerLoop }, 0, 7, time_.sF() - Fire::TimeFire1);
			TextureAsset(U"BurningLoop1")(frame * 24, 0, 24, 32).drawAt(drawPos);
		}
		else
		{
			const int frame = PeriodicStair(Duration{ Fire::TimeFire3 }, 0, 4, time_.sF() - Fire::TimeFire1 - Fire::TimeFire2PerLoop * Fire::Loop);
			TextureAsset(U"BurningEnd1")(frame * 24, 0, 24, 32).drawAt(drawPos);
		}
	}

	void WeaponFire::onCollide(Actor* other)
	{
		if (not active()) return;

		if (other->tag() == ActorTag::Block ||
			other->tag() == ActorTag::Enemy)
		{
			// これ自体はダメージを与えない
			//other->setDamage(0.5);

			// なにかに当たったら爆発して消える
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(2.0, 4.0), Random(Math::TwoPi) }, 1.0, 0.0));
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(8.0, 12.0), Random(Math::TwoPi) }, 0.6, 0.2, false));
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(8.0, 12.0), Random(Math::TwoPi) }, 0.6, 0.25, false));
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(16.0, 20.0), Random(Math::TwoPi) }, 0.3, 0.35, false));
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(16.0, 20.0), Random(Math::TwoPi) }, 0.3, 0.38, false));
			gameData_.actors.push_back(std::make_unique<WeaponExplode>(position().currentPos() + Circular{ Random(16.0, 20.0), Random(Math::TwoPi) }, 0.3, 0.41, false));

			setInactive();
		}
	}

	const Vec2& WeaponFire::getCollisionPos() const
	{
		return position();
	}

	const Collision& WeaponFire::getCollision() const
	{
		return collision_;
	}

	WeaponExplode::WeaponExplode(const Vec2& pos, double scale, double delay, bool collidable)
		:
		Weapon{ pos },
		scale_{ scale },
		delay_{ Duration{ delay }, StartImmediately::Yes, Clock() },
		collision_{},
		time_{ StartImmediately::No, Clock() }
	{
		if (collidable)
		{
			collision_.set(Circle{ Vec2{}, 22 * scale });
		}
	}

	WeaponExplode::~WeaponExplode()
	{
	}

	void WeaponExplode::update()
	{
		// 時間差
		if (delay_.reachedZero() && not time_.isRunning())
		{
			time_.start();
		}

		if (time_.sF() > Explode::LifeTime)
		{
			setInactive();
		}
	}

	void WeaponExplode::draw() const
	{
		if (not time_.isRunning()) return;

		// 白
		const double tWhite = EaseInCubic(Saturate(time_.sF() / 0.12));
		ScopedColorMul2D mul{ tWhite, 1 };
		ScopedColorAdd2D add{ 1 - tWhite, 0 };

		const double alpha = 1 - ((time_.sF() - (Explode::LifeTime - 0.095)) > 0 ? 0.8 * Periodic::Square0_1(0.03s) : 0);
		const double t = Saturate(time_.sF() / Explode::LifeTime);
		const double r = (6 + 14 * EaseOutCubic(t)) * scale_ + (1 - tWhite) * 6;

		const ColorF innerColor = Palette::Yellow.lerp(Palette::Red, 0.5 * Periodic::Sine0_1(0.08s, ClockTime()));
		const ColorF outerColor = Palette::Orange.lerp(Palette::Red, 0.4 * Periodic::Sine0_1(0.08s, ClockTime()));
		const ColorF edgeColor = Palette::Darkorange.lerp(Palette::Red, 0.5 * Periodic::Sine0_1(0.08s, ClockTime()));

		Circle{ position().currentPos().asPoint(), Math::Floor(r) }
			.draw(ColorF{ innerColor, alpha }, ColorF{ outerColor, alpha })
			.drawFrame((8 - 5 * EaseInOutCubic(t)) * scale_, ColorF{ edgeColor, alpha });
	}

	void WeaponExplode::onCollide(Actor* other)
	{
		if ((other->tag() == ActorTag::Enemy) ||
			(other->tag() == ActorTag::Block))
		{
			other->setDamage(0.4);
		}
	}

	const Vec2& WeaponExplode::getCollisionPos() const
	{
		return position();
	}

	const Collision& WeaponExplode::getCollision() const
	{
		return collision_;
	}
}

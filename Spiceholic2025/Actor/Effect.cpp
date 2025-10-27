#include "Effect.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawSprite.h"
#include "Core/Periodic.h"
#include "Actor/Player.h"

namespace Spiceholic
{
	namespace
	{
	}

	Fx::Fx(const Vec2& pos)
		:
		Actor{ pos },
		collision_{}
	{
	}

	Fx::~Fx()
	{
	}

	void Fx::update()
	{
	}

	void Fx::draw() const
	{
	}

	const Vec2& Fx::getCollisionPos() const
	{
		return position();
	}

	const Collision& Fx::getCollision() const
	{
		return collision_;
	}

	FxBlockBreak::FxBlockBreak(const Vec2& pos, bool secret)
		:
		Fx{ pos },
		timer_{ 0.4s, StartImmediately::Yes, Clock() },
		timer2_{ 0.5s, StartImmediately::No, Clock() },
		secret_{ secret }
	{
	}

	FxBlockBreak::~FxBlockBreak()
	{
	}

	void FxBlockBreak::update()
	{
		if (not active()) return;

		if (timer_ > 0.1s && not timer2_.isRunning())
		{
			timer2_.start();
		}

		if (timer_.reachedZero() && timer2_.reachedZero())
		{
			setInactive();
		}
	}

	void FxBlockBreak::draw() const
	{
		const ColorF color = secret_ ? GaugeMaxEffectColor() : ColorF{ 1 };

		if (timer_.isRunning())
		{
			const double t = timer_.progress0_1();
			const double r = 5 + 28 * EaseOutCubic(t);
			const double alpha = 0.5 + 0.5 * Periodic::Square0_1(0.04s, ClockTime());
			Circle{ position(), r }.drawFrame(8 - 7 * t, ColorF{ color, alpha });
		}

		if (timer2_.isRunning())
		{
			const double r = 8 + 64 * EaseOutQuad(timer2_.progress0_1());
			const double alpha = 1.0 - EaseInSine(timer2_.progress0_1());
			Circle{ position(), r }.drawFrame(1.0, ColorF{ color, alpha });
		}
	}

	FxTwinkle::FxTwinkle(const Vec2& pos)
		:
		Fx{ pos },
		timer_{ 0.2s, StartImmediately::Yes, Clock() },
		angle_{ Random(1, 2) * 45_deg }
	{
	}

	FxTwinkle::~FxTwinkle()
	{
	}

	void FxTwinkle::update()
	{
		if (timer_.reachedZero())
		{
			setInactive();
		}
	}

	void FxTwinkle::draw() const
	{
		const double scale = 0.1 + 0.9 * Periodic::Jump0_1(0.2s, timer_.progress0_1() * 0.2);
		Shape2D::NStar(4, 5 * scale, 2 * scale, position().currentPos().asPoint() + Vec2{.5,.5}, angle_).draw();
	}

	FxSmoke::FxSmoke(const Vec2& pos, const Actor* target, double scale, double vel, double delay)
		:
		Fx{ pos },
		target_{ target },
		scale_{ scale },
		vel_{ vel },
		timerDelay_{ Duration{ delay }, StartImmediately::Yes, Clock() },
		offset_{ target ? target->position().currentPos() - pos : Vec2{} },
		timer_{ Duration{ Random(0.2, 0.35) }, StartImmediately::No, Clock() },
		angle_{ Random(Math::TwoPi) }
	{
	}

	FxSmoke::~FxSmoke()
	{
	}

	void FxSmoke::update()
	{
		if (timerDelay_.reachedZero())
		{
			timerDelay_.set(999s);
			timerDelay_.reset();

			timer_.start();
		}

		if (timer_.reachedZero())
		{
			setInactive();
		}
	}

	void FxSmoke::draw() const
	{
		const double t10 = timer_.progress1_0();
		const double t01 = timer_.progress0_1();

		const ColorF color{ 1.0, EaseOutCubic(t10) };

		RectF{ Arg::center = (target_ ? target_->position().currentPos() : position().currentPos()) - offset_ - Vec2{ 0, EaseOutQuad(t01) * (12.0 * vel_) }, 4.5 * scale_ }
			.scaled(EaseOutSine(t01))
			.rotated(angle_ + t01 * 200_deg)
			.draw(color);
	}

	FxSmoke2::FxSmoke2(GameData& gameData, const Vec2& pos, double scale, double vel, double delay)
		:
		Fx{ pos },
		gameData_{ gameData },
		scale_{ scale },
		vel_{ vel },
		mirror_{ RandomBool() },
		timerDelay_{ Duration{ delay }, StartImmediately::Yes, Clock() },
		time_{ StartImmediately::No, Clock() }
	{
	}

	FxSmoke2::~FxSmoke2()
	{
	}

	void FxSmoke2::update()
	{
		if (timerDelay_.reachedZero())
		{
			timerDelay_.set(999s);
			timerDelay_.reset();

			time_.start();
		}

		if (time_ >= 0.4s)
		{
			setInactive();
		}

		if (time_.isRunning())
		{
			setCurrentPosition(position().currentPos() + Vec2{ 0, vel_ } * Scene::DeltaTime());
		}
	}

	void FxSmoke2::draw() const
	{
		if (time_.isRunning() && time_ < 0.4s)
		{
			Transformer2D scale{ Mat3x2::Scale(scale_, position().currentPos()) };
			DrawSprite(*gameData_.appSetting, U"Smoke", 0.4s, mirror_, position().currentPos(), time_.sF());
		}
	}

	FxPowerup::FxPowerup(GameData& gameData)
		:
		Fx{ gameData.player->position().currentPos() + Vec2{ 0, -18 }},
		gameData_{ gameData },
		time_{ StartImmediately::Yes, Clock() },
		pos_{}
	{
		pos_ = position();
	}

	FxPowerup::~FxPowerup()
	{
	}

	void FxPowerup::update()
	{
		if (time_ > 0.6s)
		{
			setInactive();
		}

		const double t = Saturate(time_.sF() / 1.0);
		setCurrentPosition(pos_ - Vec2{ -20 * EaseOutSine(t), 0 });
	}

	void FxPowerup::draw() const
	{
		const double alpha = Saturate(1.0 - EaseInQuad(time_.sF() / 1.0));//(time_ > 0.4s) ? 0.5 * Periodic::Square0_1(0.08s) : 0.3 + 0.7 * Periodic::Square0_1(0.08s);

		TextureAsset(U"Powerup")(PeriodicStair(0.08s, 0, 2, time_.sF()) * 48, 0, 48, 16).drawAt(position(), AlphaF(alpha));
	}

	FxNotice::FxNotice(GameData& gameData, const Vec2& pos)
		:
		Fx{ pos },
		gameData_{ gameData },
		time_{ StartImmediately::Yes, Clock() }
	{
	}

	FxNotice::~FxNotice()
	{
	}

	void FxNotice::update()
	{
		if (time_ > 1.2s)
		{
			setInactive();
		}
	}

	void FxNotice::draw() const
	{
		{
			ScopedColorMul2D mul{ 1, 0.5 + 0.5 * Periodic::Square0_1(0.1s, time_.sF()) };
			DrawSprite(*gameData_.appSetting, U"Notice", 0.6s, false, position().currentPos(), time_.sF());
		}
	}
}

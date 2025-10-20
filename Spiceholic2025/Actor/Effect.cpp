#include "Effect.h"
#include "Config/GameConfig.h"

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

	FxBlockBreak::FxBlockBreak(const Vec2& pos)
		:
		Fx{ pos },
		timer_{ 0.4s, StartImmediately::Yes, Clock() },
		timer2_{ 0.5s, StartImmediately::No, Clock() }
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
		if (timer_.isRunning())
		{
			const double t = timer_.progress0_1();
			const double r = 5 + 28 * EaseOutCubic(t);
			const double alpha = 0.5 + 0.5 * Periodic::Square0_1(0.04s, ClockTime());
			Circle{ position(), r }.drawFrame(8 - 7 * t, ColorF{ 1.0, alpha });
		}

		if (timer2_.isRunning())
		{
			const double r = 8 + 64 * EaseOutQuad(timer2_.progress0_1());
			const double alpha = 1.0 - EaseInSine(timer2_.progress0_1());
			Circle{ position(), r }.drawFrame(1.0, ColorF{ 1.0, alpha });
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
}

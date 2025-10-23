#include "Gauge.h"

namespace Spiceholic
{
	Gauge::Gauge(double value)
		:
		currentValue_{ value },
		displayValue_{ value },
		timeMaxEffect_{ StartImmediately::No, Clock() }
	{
	}

	double Gauge::add(double value)
	{
		currentValue_ = Saturate(currentValue_ + value);

		return currentValue_;
	}

	void Gauge::update()
	{
		if (Abs(currentValue_ - displayValue_) < 1e-3)
		{
			displayValue_ = currentValue_;
			return;
		}

		displayValue_ = Saturate(displayValue_ + (currentValue_ - displayValue_) * (60 * 0.3) * Scene::DeltaTime());
	}

	void Gauge::draw() const
	{
		// ゲージ
		const Vec2 gaugePos{ 60, 174 };
		const double gaugeLength = 95 * displayValue();

		{
			const double t = (Abs(currentValue_ - displayValue_) < 1e-3) ? 1 : 0.6 * Periodic::Square0_1(0.07s, ClockTime());
			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1-t, 0};
			TextureAsset(U"Gauge")(0, 0, gaugeLength, 6).draw(gaugePos + Vec2{ 21, 7 }, ColorF{ 1 - 0.08 * Periodic::Jump0_1(0.3s, ClockTime()) });
		}

		// ゲージ枠
		TextureAsset(U"GaugeFrame").draw(gaugePos, ColorF{ 1 - 0.08 * Periodic::Sine0_1(0.2s, ClockTime()) });

		// ゲージMAXエフェクト
		if (timeMaxEffect_.isRunning() && timeMaxEffect_ < 0.95s)
		{
			const double t = Saturate(timeMaxEffect_.sF() / 0.95);
			const double r = 4 + 24 * EaseOutCubic(t);
			const double alpha = (timeMaxEffect_ > 0.5s) ? Periodic::Square0_1(0.05s, ClockTime()) : 1.0;
			const Vec2 pos{ 60 + 21 + 95, 174 + 7 + 6 / 2 };
			const ColorF color{ Palette::Red.lerp(Palette::Yellow, 0.5 * Periodic::Square0_1(0.07s, ClockTime())), alpha };
			Circle{ pos, r }.drawFrame(8 - 7 * EaseOutSine(t), color);

			Line{ pos, pos + Circular{ 400, 10_deg + 15_deg * t } }.draw(7 - 6.5 * EaseInOutQuad(t), color);
			Line{ pos, pos + Circular{ 400, -40_deg + 24_deg * t } }.draw(4 - 3.5 * EaseInOutCubic(t), color);
			Line{ pos, pos + Circular{ 400, 60_deg + 13_deg * t } }.draw(5 - 4.5 * EaseInOutCirc(t), color);
			Line{ pos, pos + Circular{ 400, -90_deg + 20_deg * t } }.draw(7 - 7.5 * EaseInOutQuad(t), color);

		}
	}

	double Gauge::getValue() const
	{
		return currentValue_;
	}

	double Gauge::displayValue() const
	{
		return displayValue_;
	}

	bool Gauge::isPoweredUp() const
	{
		return currentValue_ > 0.499;
	}

	void Gauge::startDrawMaxEffect()
	{
		timeMaxEffect_.restart();
	}
}

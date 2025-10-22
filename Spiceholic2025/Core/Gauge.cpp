#include "Gauge.h"

namespace Spiceholic
{
	Gauge::Gauge(double value)
		:
		currentValue_{ value },
		displayValue_{ value }
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
	}

	double Gauge::getValue() const
	{
		return currentValue_;
	}

	double Gauge::displayValue() const
	{
		return displayValue_;
	}
}

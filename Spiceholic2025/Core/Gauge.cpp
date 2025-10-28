#include "Gauge.h"
#include "Core/Color.h"

namespace Spiceholic
{
	Gauge::Gauge(int32 value)
		:
		currentValue_{ value },
		displayValue_{ value * 1.0 },
		timeMaxEffect_{ StartImmediately::No, Clock() }
	{
	}

	int32 Gauge::add(int32 value)
	{
		currentValue_ = Clamp(currentValue_ + value, 0, 100);

		return currentValue_;
	}

	void Gauge::update()
	{
		if (Abs(currentValue_ - displayValue_) < 1e-3)
		{
			displayValue_ = currentValue_;
			return;
		}

		displayValue_ = Clamp(displayValue_ + (currentValue_ - displayValue_) * (60 * 0.3) * Scene::DeltaTime(), 0.0, 100.0);
	}

	void Gauge::draw() const
	{
		// ゲージ
		const Vec2 gaugePos{ 60 - 4, 174 };
		const double gaugeLength = 100 * displayValue() / 100;

		{
			const double t = (Abs(currentValue_ - displayValue_) < 1e-3) ? 1 : 0.8 * Periodic::Pulse0_1(0.06s, 0.8, ClockTime());
			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1 - t, 0 };
			TextureAsset(U"Gauge")(0, 0, gaugeLength, 6).draw(gaugePos + Vec2{ 21, 7 }, ColorF{ 1 - 0.08 * Periodic::Jump0_1(0.3s, ClockTime()) });
		}

		// ゲージMAXエフェクト中
		const bool onMaxEffect = (timeMaxEffect_.isRunning() && timeMaxEffect_ < 0.95s);
		const double tMaxEffect = Saturate(timeMaxEffect_.sF() / 0.95);

		// ゲージ枠
		const ColorF gaugeFrameColor = onMaxEffect ? GaugeMaxEffectColor().lerp(Palette::White, tMaxEffect) : ((currentValue_ >= 50) ? ColorF{1,1,0.93}.lerp(Palette::Darkorange, 0.7 * Periodic::Pulse0_1(0.20s, 0.3)) : ColorF{ 1 - 0.08 * Periodic::Sine0_1(0.2s, ClockTime()) });
		TextureAsset(U"GaugeFrame").draw(gaugePos, gaugeFrameColor);

		// ゲージMAXエフェクト
		if (onMaxEffect)
		{
			const Vec2 pos{ 60 + 21 + 95, 174 + 7 + 6 / 2 };
			const double r = 4 + 32 * EaseOutExpo(Saturate(tMaxEffect * 1.1));
			const double alpha = (timeMaxEffect_ > 0.5s) ? Periodic::Square0_1(0.05s, ClockTime()) : 1.0;
			const ColorF color{ GaugeMaxEffectColor(), alpha };
			Circle{ pos, r }.drawFrame(8 - 7 * EaseOutCubic(tMaxEffect), color);

			for (int i = 0; i < 2; ++i)
			{
				Line{ pos, pos + Circular{ 400, i * 180_deg + 10_deg + 15_deg * tMaxEffect } }.draw(7 - 6.5 * EaseInOutQuad(tMaxEffect), color);
				Line{ pos, pos + Circular{ 400, i * 180_deg + -40_deg + 44_deg * tMaxEffect } }.draw(4 - 3.5 * EaseInOutCubic(tMaxEffect), color);
				Line{ pos, pos + Circular{ 400, i * 180_deg + 60_deg + 13_deg * tMaxEffect } }.draw(5 - 4.5 * EaseInOutCirc(tMaxEffect), color);
				Line{ pos, pos + Circular{ 400, i * 180_deg + -90_deg + 58_deg * tMaxEffect } }.draw(7 - 7.5 * EaseInOutQuad(tMaxEffect), color);
			}
		}
	}

	int32 Gauge::getValue() const
	{
		return currentValue_;
	}

	double Gauge::displayValue() const
	{
		return displayValue_;
	}

	bool Gauge::isPoweredUp() const
	{
		return currentValue_ >= 50;
	}

	void Gauge::startDrawMaxEffect()
	{
		timeMaxEffect_.restart();
	}
}

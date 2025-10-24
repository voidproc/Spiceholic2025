#pragma once

namespace Spiceholic
{
	inline constexpr Color LightYellow = Palette::Yellow.lerp(Palette::White, 0.3);

	// オレンジ点滅カラーなど
	ColorF GaugeMaxEffectColor();
}

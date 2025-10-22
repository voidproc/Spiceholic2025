#pragma once

namespace Spiceholic
{
	inline constexpr ColorF TextShadowColor{ 0, 0.5 };

	RectF DrawText(AssetNameView fontName, StringView text, Arg::center_<Vec2> pos, const ColorF& color, const ColorF& shadowColor = TextShadowColor);
	RectF DrawText(AssetNameView fontName, StringView text, Arg::leftCenter_<Vec2> pos, const ColorF& color, const ColorF& shadowColor = TextShadowColor);
	RectF DrawText(AssetNameView fontName, StringView text, Arg::rightCenter_<Vec2> pos, const ColorF& color, const ColorF& shadowColor = TextShadowColor);
}

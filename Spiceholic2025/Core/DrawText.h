#pragma once

namespace Spiceholic
{
	RectF DrawText(AssetNameView fontName, StringView text, Arg::center_<Vec2> pos, const ColorF& color);
	RectF DrawText(AssetNameView fontName, StringView text, Arg::leftCenter_<Vec2> pos, const ColorF& color);
	RectF DrawText(AssetNameView fontName, StringView text, Arg::rightCenter_<Vec2> pos, const ColorF& color);
}

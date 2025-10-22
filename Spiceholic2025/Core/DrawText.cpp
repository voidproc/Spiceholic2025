#include "DrawText.h"

namespace Spiceholic
{
	namespace
	{
	}

	RectF DrawText(AssetNameView fontName, StringView text, Arg::center_<Vec2> pos, const ColorF& color, const ColorF& shadowColor)
	{
		FontAsset(fontName)(text).drawAt(*pos + Vec2::One(), shadowColor); // 影
		return FontAsset(fontName)(text).drawAt(*pos, color);
	}

	RectF DrawText(AssetNameView fontName, StringView text, Arg::leftCenter_<Vec2> pos, const ColorF& color, const ColorF& shadowColor)
	{
		FontAsset(fontName)(text).draw(Arg::leftCenter = *pos + Vec2::One(), shadowColor); // 影
		return FontAsset(fontName)(text).draw(Arg::leftCenter = *pos, color);
	}

	RectF DrawText(AssetNameView fontName, StringView text, Arg::rightCenter_<Vec2> pos, const ColorF& color, const ColorF& shadowColor)
	{
		FontAsset(fontName)(text).draw(Arg::rightCenter = *pos + Vec2::One(), shadowColor); // 影
		return FontAsset(fontName)(text).draw(Arg::rightCenter = *pos, color);
	}
}

#pragma once

namespace Spiceholic
{
	void DrawSprite(const AppSetting& appSetting, StringView spriteName, const Duration& animSpeed, bool mirror, const Vec2& pos, double time = ClockTime());
}

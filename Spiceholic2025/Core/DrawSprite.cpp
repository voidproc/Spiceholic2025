#include "DrawSprite.h"
#include "Core/Periodic.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	void DrawSprite(const AppSetting& appSetting, StringView spriteName, const Duration& animSpeed, bool mirror, const Vec2& pos, double time)
	{
		const SpriteInfo& sprite = appSetting.get().sprite.at(spriteName);

		const int animFrame = PeriodicStair(animSpeed, 0, sprite.count - 1, time);
		TextureAsset(sprite.textureName)(sprite.pos + Vec2{ animFrame * sprite.size, 0 }, sprite.size, sprite.size)
			.mirrored(mirror)
			.drawAt(pos);
	}
}

#pragma once

namespace Spiceholic
{
	inline constexpr Size SceneSize{ 240, 192 };
	inline constexpr Rect SceneRect{ SceneSize };
	inline constexpr Vec2 SceneCenter{ SceneRect.center() };
}

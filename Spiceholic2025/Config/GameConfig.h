#pragma once

namespace Spiceholic
{
	inline constexpr Size SceneSize{ 240, 192 };
	inline constexpr Rect SceneRect{ SceneSize };
	inline constexpr Vec2 SceneCenter{ SceneRect.center() };
	inline constexpr int32 TileSize = 16;
	inline constexpr Size ActorsFieldViewportSize = SceneSize - Size{ 0, TileSize * 2 };
	inline constexpr Rect ActorsFieldViewportRect{ 0, TileSize, ActorsFieldViewportSize };
	inline constexpr FilePathView AppSettingJsonPath = U"setting/setting.json";
	inline constexpr FilePathView UserSettingFilePath = U"setting.ini";
	inline constexpr FilePathView InputSettingFilePath = U"input.ini";
	inline constexpr FilePathView RecordsFilePath = U"records.dat";
}

//#define DEBUG_MODE

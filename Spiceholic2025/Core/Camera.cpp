#include "Camera.h"
#include "Actor/Player.h"
#include "Config/GameConfig.h"
#include "Setting/AppSetting.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
	RectF CameraRect(const GameData& gameData)
	{
		const SizeF mapSize = gameData.stageData->gridSize * TileSize;
		const Vec2 playerPos = gameData.player->position().currentPos();
		RectF cameraRect{ Arg::center = playerPos, ActorsFieldViewportSize };

		if (gameData.stageData->openedSecretRoute)
		{
			cameraRect.x = Clamp(cameraRect.x, 0.0, mapSize.x - ActorsFieldViewportSize.x);
			cameraRect.y = Clamp(cameraRect.y, 0.0, mapSize.y - ActorsFieldViewportSize.y);
		}
		else
		{
			cameraRect.x = Clamp(cameraRect.x, gameData.stageData->cameraTopLeft.x, gameData.stageData->cameraBottomRight.x - ActorsFieldViewportSize.x);
			cameraRect.y = Clamp(cameraRect.y, gameData.stageData->cameraTopLeft.y, gameData.stageData->cameraBottomRight.y - ActorsFieldViewportSize.y);
		}

		return cameraRect;
	}

	Transformer2D CameraTransform(const GameData& gameData)
	{
		const Vec2 cameraMove = -CameraRect(gameData).pos;
		return Transformer2D{ Mat3x2::Translate(cameraMove) };
	}
}

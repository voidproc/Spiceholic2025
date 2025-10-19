#pragma once

#include "Actor/ActorType.h"

namespace Spiceholic
{
	struct ActorSpawnInfo
	{
		ActorType type;
		Vec2 position;
		double time;
		double life;
	};

	struct StageData
	{
		// ステージ名
		String name;

		// ステージの大きさ（グリッド列数・行数）
		Size gridSize;

		// プレイヤーの初期位置
		Vec2 playerStartPos;

		// アクター生成情報
		Array<ActorSpawnInfo> actorSpawns;
	};

	// 指定されたステージ番号のステージデータを読み込む
	void LoadStage(const String& stageID, StageData& stageData);
}

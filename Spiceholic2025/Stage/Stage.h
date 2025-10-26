#pragma once

#include "Actor/ActorType.h"
#include "Actor/Direction.h"

namespace Spiceholic
{
	struct ActorSpawnInfo
	{
		ActorType type;
		Vec2 position;
		double time;
		double life;

		// 向いている方向や進む方向など
		Direction direction;

		// 敵の挙動パターンのバリエーションなど
		int32 subType;

		// 所持アイテム
		String bringItems;
		// 鍵を持っている
		//bool hasKey;

		// シークレット
		bool secretRoute;
	};

	struct StageData
	{
		// ステージ名
		String name;

		// このステージのステージID
		String stageID;

		// 次のステージID
		String nextStageID;

		// ステージの大きさ（グリッド列数・行数）
		Size gridSize;

		// プレイヤーの初期位置
		Vec2 playerStartPos;

		// デフォルトのカメラ移動可能領域
		Vec2 cameraTopLeft;
		Vec2 cameraBottomRight;

		// アクター生成情報
		Array<ActorSpawnInfo> actorSpawns;

		bool openedSecretRoute;
	};

	// 指定されたステージ番号のステージデータを読み込む
	void LoadStage(const String& stageID, StageData& stageData);
}

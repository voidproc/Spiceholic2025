#pragma once

namespace Spiceholic
{
	class AppSetting;
	class UserSetting;
	class ActionInput;
	class Player;
	class Block;
	class Actor;
	struct StageData;
	class Gauge;
	class Score;

	struct GameData
	{
		// アプリケーション設定
		std::unique_ptr<AppSetting> appSetting;

		// ユーザー設定
		std::unique_ptr<UserSetting> userSetting;

		// 入力
		std::unique_ptr<ActionInput> actionInput;

		// ステージ
		std::unique_ptr<StageData> stageData;

		// 次のステージID
		String nextStageID;

		// 辛ゲージ
		std::unique_ptr<Gauge> gauge;

		// スコア
		std::unique_ptr<Score> score;

		// プレイヤー
		std::unique_ptr<Player> player;

		// ブロック
		Array<std::unique_ptr<Block>> blocks;

		// その他（敵・アイテムなど）
		Array<std::unique_ptr<Actor>> actors;

		GameData();

		~GameData();
	};
}

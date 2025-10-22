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

	struct GameData
	{
		// アプリケーション設定
		std::unique_ptr<AppSetting> appSetting;

		// ユーザー設定
		std::unique_ptr<UserSetting> userSetting;

		// 入力
		std::unique_ptr<ActionInput> actionInput;

		// 入力
		std::unique_ptr<StageData> stageData;

		// プレイヤー
		std::unique_ptr<Player> player;

		// ブロック
		Array<std::unique_ptr<Block>> blocks;

		// その他（敵・アイテムなど）
		Array<std::unique_ptr<Actor>> actors;

		// 炎ゲージ
		double gauge;

		GameData();

		~GameData();
	};
}

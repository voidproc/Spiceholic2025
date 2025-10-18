#pragma once

namespace Spiceholic
{
	class AppSetting;
	class Player;
	//class Block;
	class Actor;

	struct GameData
	{
		// アプリケーション設定
		std::unique_ptr<AppSetting> appSetting;

		// プレイヤー
		std::unique_ptr<Player> player;

		// ブロック
		//Array<std::unique_ptr<Block>> blocks;

		// その他（敵・アイテムなど）
		Array<std::unique_ptr<Actor>> actors;

		GameData();

		~GameData();
	};
}

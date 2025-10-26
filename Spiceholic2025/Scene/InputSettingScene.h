#pragma once

#include "CustomScene.h"
#include "Core/Menu.h"

namespace Spiceholic
{
	enum class Action;

	class InputSettingScene : public CustomScene
	{
	public:
		inline static const String Name = U"InputSettingScene";

		InputSettingScene(const InitData& init);

		~InputSettingScene() override;

		void update() override;

		void draw() const override;

	private:
		// 入力割り当て設定をInputSceneからコピーしてくる
		void copySettingToTemporaryData_();

		// キー入力割り当て処理
		void assignKey_(Action action);

		// コントローラ入力割り当て処理
		void assignButton_(Action action);

		// 設定を保存
		void saveSetting_();

		// 描画用
		void startTimerSetKey_(Action action, const Duration& time);
		void startTimerSetButton_(Action action, const Duration& time);

		Menu menu_;

		// 入力割り当て設定（キー）
		// このシーン開始時にInputSettingからコピーされ、シーン終了時に書き戻される
		HashTable<Action, Input> tempKeyBindings_;

		// 入力割り当て設定（ボタン）
		// このシーン開始時にInputSettingからコピーされ、シーン終了時に書き戻される
		HashTable<Action, Input> tempButtonBindings_;

		// 描画用
		Timer timerChangeMenuIndex_;
		Timer timerSetBinding_;
		Array<Timer> timerSetKey_;
		Array<Timer> timerSetButton_;
	};
}


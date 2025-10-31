#pragma once

namespace Spiceholic
{
	// Actionは、ユーザ入力に対応するゲーム内でのユーザ操作
	// １つのActionにキーボード・ゲームコントローラのキー・ボタンが割り当てられる
	enum class Action
	{
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,
		Attack,
		Pause,

		Decide = Attack,
		Invalid = 0xffff,
	};

	// 定義されているActionのリスト
	inline constexpr std::array<Action, 7> ActionList = { {
		Action::MoveUp,
		Action::MoveDown,
		Action::MoveLeft,
		Action::MoveRight,
		Action::Attack,
		Action::Pause,
	} };

	// Actionの名称
	StringView GetActionName(Action action);

	// 移動に関するActionかどうか
	bool IsMoveAction(Action action);
}

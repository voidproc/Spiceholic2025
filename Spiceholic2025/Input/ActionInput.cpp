#include "ActionInput.h"
#include "Input.h"

namespace Spiceholic
{
	ActionInput::ActionInput()
		: inputSetting_{}
	{
		// ユーザ設定の読み込み前に、とりあえず0番目のゲームコントローラにより初期設定
		applyDefault(0);
	}

	bool ActionInput::down(Action action) const
	{
		// TODO: 移動アクション入力（スティック）については個別判定
		//...

		return inputSetting_.getInputs(action).down();
	}

	bool ActionInput::up(Action action) const
	{
		// TODO: 移動アクション入力（スティック）については個別判定
		//...

		return inputSetting_.getInputs(action).up();
	}

	bool ActionInput::pressed(Action action) const
	{
		// TODO: 移動アクション入力（スティック）については個別判定
		//...

		return inputSetting_.getInputs(action).pressed();
	}

	uint8 ActionInput::getControllerId() const
	{
		return inputSetting_.controllerId();
	}

	void ActionInput::setControllerId(uint8 id)
	{
		inputSetting_.setControllerId(id);
	}

	const InputGroup& ActionInput::getInputs(Action action) const
	{
		return inputSetting_.getInputs(action);
	}

	void ActionInput::setInputs(Action action, const InputGroup& input)
	{
		inputSetting_.setInputs(action, input);
	}

	void ActionInput::load(FilePathView path)
	{
		inputSetting_.load(path);
	}

	void ActionInput::save(FilePathView path)
	{
		inputSetting_.save(path);
	}

	void ActionInput::applyDefault(uint8 playerIndex)
	{
		// TODO:
		// - プレイヤーIDは暫定で 0 を使用
		// - Gamepadの場合、軸の状態を個別に判定する必要あり
		// - XInputの場合、スティックが押されている方向を個別に判定する必要あり

		// メモ:
		// 移動アクション用のキーは、設定画面での個別割り当てを想定しない

		inputSetting_.setInputs(Action::MoveLeft, KeyLeft | DefaultMoveLeftButtons(playerIndex));
		inputSetting_.setInputs(Action::MoveRight, KeyRight | DefaultMoveRightButtons(playerIndex));
		inputSetting_.setInputs(Action::MoveUp, KeyUp | DefaultMoveUpButtons(playerIndex));
		inputSetting_.setInputs(Action::MoveDown, KeyDown | DefaultMoveDownButtons(playerIndex));
		inputSetting_.setInputs(Action::Attack, KeyZ | Input{ InputDeviceType::Gamepad, (uint8)0, playerIndex });
		inputSetting_.setInputs(Action::Cancel, KeyX | Input{ InputDeviceType::Gamepad, (uint8)1, playerIndex });
		inputSetting_.setInputs(Action::Pause, KeyEscape | Input{ InputDeviceType::Gamepad, (uint8)2, playerIndex });
	}

	void ActionInput::update()
	{
		// TODO: 移動アクション入力（スティック）の up, down を判定するための更新処理
		//...
	}
}

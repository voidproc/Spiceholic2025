#pragma once

#include "Action.h"
#include "Setting/InputSetting.h"

namespace Spiceholic
{
	// Actionに割り当てられているユーザ入力の状態判定を行う
	// ※割り当て設定はInputSettingが保持する
	class ActionInput
	{
	public:
		ActionInput();

		bool down(Action action) const;

		bool up(Action action) const;

		bool pressed(Action action) const;

		uint8 getControllerId() const;

		void setControllerId(uint8 id);

		const InputGroup& getInputs(Action action) const;

		void setInputs(Action action, const InputGroup& input);

		void load(FilePathView path);

		void save(FilePathView path);

		// デフォルト設定を適用する
		// InputSettingに設定を書き込む
		void applyDefault(uint8 playerIndex);

		void update();

	private:
		InputSetting inputSetting_;

	};
}

#pragma once

namespace Spiceholic
{
	enum class Action;

	// Actionに対応するユーザ入力設定を保持する
	class InputSetting
	{
	public:
		InputSetting();

		void load(FilePathView path);

		void save(FilePathView path);

		uint8 controllerId() const;

		void setControllerId(uint8 id);

		// Actionに割り当てられている入力を取得
		const InputGroup& getInputs(Action action) const;

		// Actionに入力を割り当てる
		void setInputs(Action action, const InputGroup& input);

	private:
		uint8 controllerId_;
		HashTable<Action, InputGroup> input_;
	};
}

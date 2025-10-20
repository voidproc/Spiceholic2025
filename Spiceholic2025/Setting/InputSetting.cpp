#include "InputSetting.h"
#include "Input/Action.h"

namespace Spiceholic
{
	namespace
	{
		const InputGroup EmptyInput{};

		Input Uint32ToInput(uint32 num)
		{
			union
			{
				uint32 num;
				Input input;
			} u{ num };

			return u.input;
		}

		InputGroup ArrayToInputGroup(const Array<Input>& inputs)
		{
			InputGroup g;

			for (const auto input : inputs)
			{
				g = g | input;
			}

			return g;
		}
	}

	InputSetting::InputSetting()
		: controllerId_{ 0 }
		, input_{}
	{
	}

	void InputSetting::load(FilePathView path)
	{
		INI ini{ path };
		if (not ini) return;

		controllerId_ = ini.getOr<uint8>(U"", U"ControllerID", 0);

		for (const auto& action : ActionList)
		{
			const auto val = ini.getOpt<String>(U"", GetActionName(action));
			if (not val) continue;

			InputGroup g;

			try
			{
				// コンマ区切りの数値をInputの配列に変換
				const auto inputs = (*val).split(U',').map(Parse<uint32>).map(Uint32ToInput);

				g = ArrayToInputGroup(inputs);
			}
			catch (const ParseError&)
			{
				Logger << U"InputSetting::load() ParseError";
			}
			
			if (not g.inputs().empty())
			{
				input_[action] = g;
			}
		}
	}

	void InputSetting::save(FilePathView path)
	{
		INI ini{};

		ini.writeGlobal(U"ControllerID", controllerId());

		for (const auto& action : ActionList)
		{
			const auto inputsStr = input_.at(action).inputs()
				.map([](const Input& input) { return Format(input.asUint32()); })
				.join(U",", U"", U"");

			ini.writeGlobal(GetActionName(action), inputsStr);
		}

		ini.save(path);
	}

	uint8 InputSetting::controllerId() const
	{
		return controllerId_;
	}

	void InputSetting::setControllerId(uint8 id)
	{
		controllerId_ = id;
	}

	const InputGroup& InputSetting::getInputs(Action action) const
	{
		return input_.contains(action) ? input_.at(action) : EmptyInput;
	}

	void InputSetting::setInputs(Action action, const InputGroup& input)
	{
		input_[action] = input;
	}
}

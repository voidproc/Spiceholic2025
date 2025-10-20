#include "Input.h"

namespace Spiceholic
{
	String GetInputName(const Input& input)
	{
		if (input.deviceType() == InputDeviceType::Gamepad)
		{
			switch (input.code())
			{
			case 0x80: return U"pov:Up";
			case 0x82: return U"pov:Down";
			case 0x83: return U"pov:Left";
			case 0x81: return U"pov:Right";
			}

			return U"{}"_fmt(input.code() + 1);
		}
		else if (input.deviceType() == InputDeviceType::XInput)
		{
			switch (input.code())
			{
			case 0: return U"x:Up";
			case 1: return U"x:Down";
			case 2: return U"x:Left";
			case 3: return U"x:Right";
			case 4: return U"Start";
			case 5: return U"Back";
			case 6: return U"LT";
			case 7: return U"RT";
			case 8: return U"LB";
			case 9: return U"RB";
			case 12: return U"A";
			case 13: return U"B";
			case 14: return U"X";
			case 15: return U"Y";
			}
		}
		else if (input.deviceType() == InputDeviceType::Keyboard)
		{
			return input.name();
		}

		return U"";
	}

	Array<Input> GetAllControllerInputs(uint8 playerIndex)
	{
		Array<Input> pressedInputs;

		const auto pushPressedInput = [](const Input& input, Array<Input>& pressedInputs)
			{
				if (input.down())
				{
					pressedInputs.push_back(input);
				}
			};

		auto gamepad = Gamepad(playerIndex);

		// ※ゲームコントローラの移動用ボタンは固定で割り当てるので検出対象としない
		//pushPressedInput(gamepad.povUp, pressedInputs);
		//pushPressedInput(gamepad.povDown, pressedInputs);
		//pushPressedInput(gamepad.povLeft, pressedInputs);
		//pushPressedInput(gamepad.povRight, pressedInputs);

		for (const auto& button : gamepad.buttons)
		{
			if (button.down())
			{
				pressedInputs.push_back(button);
			}
		}

		auto xinput = XInput(playerIndex);

		// ※ゲームコントローラの移動用ボタンは固定で割り当てるので検出対象としない
		//pushPressedInput(xinput.buttonUp, pressedInputs);
		//pushPressedInput(xinput.buttonDown, pressedInputs);
		//pushPressedInput(xinput.buttonLeft, pressedInputs);
		//pushPressedInput(xinput.buttonRight, pressedInputs);

		pushPressedInput(xinput.buttonStart, pressedInputs);
		pushPressedInput(xinput.buttonBack, pressedInputs);
		pushPressedInput(xinput.buttonLThumb, pressedInputs);
		pushPressedInput(xinput.buttonRThumb, pressedInputs);
		pushPressedInput(xinput.buttonLB, pressedInputs);
		pushPressedInput(xinput.buttonRB, pressedInputs);
		pushPressedInput(xinput.buttonA, pressedInputs);
		pushPressedInput(xinput.buttonB, pressedInputs);
		pushPressedInput(xinput.buttonX, pressedInputs);
		pushPressedInput(xinput.buttonY, pressedInputs);

		return pressedInputs;
	}

	InputGroup DefaultMoveUpButtons(uint8 playerIndex)
	{
		return Gamepad(playerIndex).povUp | XInput(playerIndex).buttonUp;
	}

	InputGroup DefaultMoveDownButtons(uint8 playerIndex)
	{
		return Gamepad(playerIndex).povDown | XInput(playerIndex).buttonDown;
	}

	InputGroup DefaultMoveLeftButtons(uint8 playerIndex)
	{
		return Gamepad(playerIndex).povLeft | XInput(playerIndex).buttonLeft;
	}

	InputGroup DefaultMoveRightButtons(uint8 playerIndex)
	{
		return Gamepad(playerIndex).povRight | XInput(playerIndex).buttonRight;
	}
}

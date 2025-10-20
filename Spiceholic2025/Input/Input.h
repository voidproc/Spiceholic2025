#pragma once

namespace Spiceholic
{
	// Inputの名称
	// キーボードの場合は input.name()
	String GetInputName(const Input& input);

	// このフレームで押された（down() == true）ゲームコントローラのボタンを返す 
	Array<Input> GetAllControllerInputs(uint8 playerIndex);

	// デフォルトの移動用ゲームコントローラ入力
	InputGroup DefaultMoveUpButtons(uint8 playerIndex);
	// デフォルトの移動用ゲームコントローラ入力
	InputGroup DefaultMoveDownButtons(uint8 playerIndex);
	// デフォルトの移動用ゲームコントローラ入力
	InputGroup DefaultMoveLeftButtons(uint8 playerIndex);
	// デフォルトの移動用ゲームコントローラ入力
	InputGroup DefaultMoveRightButtons(uint8 playerIndex);

}

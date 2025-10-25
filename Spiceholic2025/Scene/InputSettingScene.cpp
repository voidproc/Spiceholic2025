#include "InputSettingScene.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/DrawText.h"
#include "Input/ActionInput.h"
#include "Input/Input.h"

namespace Spiceholic
{
	namespace
	{
		namespace Layout
		{
			constexpr int LineHeight = 14;
			constexpr double LabelColumnLeftCenterX = 12;
			constexpr double KeyColumnCenterX = SceneSize.x / 2 + 32;
			constexpr double ButtonColumnCenterX = SceneSize.x - 32;
		}

		namespace Theme
		{
			constexpr ColorF BgColor = Palette::Indianred.lerp(Palette::Darkred, 0.5);
			constexpr ColorF TitleTextColor = Palette::Whitesmoke;
			constexpr ColorF LabelColor = Palette::Lightcoral.lerp(Palette::Yellow, 0.3);
			constexpr ColorF ValueColor = Palette::Lightcoral.lerp(Palette::White, 0.3);
			constexpr ColorF ValueChangeColor = Palette::Yellow;
			constexpr ColorF SelectedLabelColor = Palette::Yellow.lerp(Palette::White, 0.5);
			constexpr ColorF SelectedColor = Palette::Whitesmoke;
			constexpr ColorF DisabledValueColor = Palette::Lightcoral.lerp(Palette::White, 0.0);
		}

		constexpr size_t MaxControllerID = 8 - 1;

		struct InputSettingField
		{
			enum class Type
			{
				Controller,
				Binding,
				Back,
			};

			Type type;
			StringView text;
			Action action;
		};

		constexpr std::array<InputSettingField, 9> SettingFieldList = { {
			{ InputSettingField::Type::Controller, U"Controller ID"_sv, Action::Invalid },
			{ InputSettingField::Type::Binding, U"MoveUp"_sv, Action::MoveUp },
			{ InputSettingField::Type::Binding, U"MoveDown"_sv, Action::MoveDown },
			{ InputSettingField::Type::Binding, U"MoveLeft"_sv, Action::MoveLeft },
			{ InputSettingField::Type::Binding, U"MoveRight"_sv, Action::MoveRight },
			{ InputSettingField::Type::Binding, U"Attack"_sv, Action::Attack },
			{ InputSettingField::Type::Binding, U"Cancel"_sv, Action::Cancel },
			{ InputSettingField::Type::Binding, U"Pause"_sv, Action::Pause },
			{ InputSettingField::Type::Back, U"<< Back"_sv, Action::Invalid },
		} };

		void ChangeSettingValue(ActionInput& actionInput, const InputSettingField& targetField, int32 valueDiff1_1)
		{
			if (targetField.type == InputSettingField::Type::Controller)
			{
				const uint8 controllerId = actionInput.getControllerId();
				const uint8 newId = static_cast<uint8>(Clamp(static_cast<int32>(controllerId) + valueDiff1_1, 0, 8));

				if (controllerId != newId)
				{
					actionInput.setControllerId(newId);
				}
			}
		}

		bool IsJustPressedMoveActionInputs(const ActionInput& actionInput)
		{
			if (actionInput.down(Action::MoveUp) ||
				actionInput.down(Action::MoveDown) ||
				actionInput.down(Action::MoveLeft) ||
				actionInput.down(Action::MoveRight))
			{
				return true;
			}

			//...

			return false;
		}
	}

	InputSettingScene::InputSettingScene(const InitData& init)
		:
		CustomScene{ init },
		selectedFieldIndex_{ 0 },
		timerChangeMenuIndex_{},
		timerSetBinding_{},
		timerSetKey_{ SettingFieldList.size() },
		timerSetButton_{ SettingFieldList.size() },
		tempKeyBindings_{},
		tempButtonBindings_{}
	{
		// 入力割り当て設定画面でだけ保持する、入力割り当て設定のコピーを作成
		// 入力割り当て設定画面では、ユーザがボタンを割り当て操作した際にこのコピーに対して変更を行い、
		// 設定を保存する際に、実際の設定へ反映する
		copySettingToTemporaryData_();
	}

	InputSettingScene::~InputSettingScene()
	{
	}

	void InputSettingScene::update()
	{
		// カーソル移動
		if (getData().actionInput->down(Action::MoveUp))
		{
			selectPrevious_();

			// 描画用
			timerChangeMenuIndex_.restart(0.1s);

			// SE
			PlayAudioOneShot(U"Select2");
		}
		else if (getData().actionInput->down(Action::MoveDown))
		{
			selectNext_();

			// 描画用
			timerChangeMenuIndex_.restart(0.1s);

			// SE
			PlayAudioOneShot(U"Select2");
		}

		// 操作対象
		const auto& selectedField = SettingFieldList[selectedFieldIndex_];

		// 選択項目に対する決定操作
		if (getData().actionInput->down(Action::Decide))
		{
			if (selectedField.type == InputSettingField::Type::Back)
			{
				// SE
				PlayAudioOneShot(U"Decide1");

				// "Back"が選択されたので設定を保存してシーン変更
				saveSetting_();
				changeScene(U"TitleScene", 0);
				return;
			}
		}

		// コントローラID選択
		if (selectedField.type == InputSettingField::Type::Controller)
		{
			const int32 valueDiff = 0 + getData().actionInput->down(Action::MoveRight) - getData().actionInput->down(Action::MoveLeft);
			ChangeSettingValue(*getData().actionInput, selectedField, valueDiff);

			if (valueDiff != 0)
			{
				// 描画用
				timerSetBinding_.restart(0.2s);
				timerSetKey_[selectedFieldIndex_].restart(0.3s);

				// SE
				PlayAudioOneShot(U"Select1");
			}
		}

		// 以降はキー割り当て操作なので、
		// アクションが割り振られていない行は無視
		if (selectedField.action == Action::Invalid) return;

		// 移動アクションが押されている場合は設定しない
		if (IsJustPressedMoveActionInputs(*getData().actionInput)) return;

		// 入力割り当て操作
		const auto action = selectedField.action;
		assignKey_(action);
		assignButton_(action);
	}

	void InputSettingScene::draw() const
	{
		const auto drawLineHighlight = [](const double centerY, const double alpha, const double timeMoveCursor1_0)
			{
				const ColorF color{ 1.0, (0.2 + 0.2 * Periodic::Jump0_1(0.75s)) * (0.8 + 0.2 * Periodic::Square0_1(32ms)) * alpha };
				RectF{ Arg::center = SceneRect.center().withY(centerY), SizeF{ SceneSize.x, Layout::LineHeight - 4 * timeMoveCursor1_0 }}.draw(color);
			};

		const auto vibrateY_whenMoveCursor = [](double timer1_0)
			{
				return 2 * Periodic::Sine1_1(0.01s) * timer1_0;
			};

		const auto vibrateX_whenBinding = [](double timer1_0)
			{
				return 2 * Periodic::Sine1_1(0.01s) * timer1_0;
			};

		const auto valueColor_whenBinding = [](const ColorF& baseColor)
			{
				return baseColor.lerp(Palette::Black, 0.3).lerp(Theme::ValueChangeColor, Periodic::Square0_1(0.034s));
			};

		// BG
		SceneRect.draw(Theme::BgColor);

		// シーンタイトル
		DrawText(U"px7812", U"Input Setting", Arg::center = SceneRect.center().withY(Layout::LineHeight * 1), Theme::TitleTextColor);

		// 各メニュー項目の描画
		for (const auto [index, item] : Indexed(SettingFieldList))
		{
			const bool selected = (index == selectedFieldIndex_);

			const double posY = Layout::LineHeight * (3.0 + index + (index > 0 ? 0.5 : 0) + (index == SettingFieldList.size() - 1 ? 0.5 : 0));
			const auto itemCenter = SceneRect.center().withY(posY);
			ColorF keyColor = Theme::ValueColor;
			ColorF buttonColor = Theme::ValueColor;

			// 選択行
			if (selected)
			{
				keyColor = buttonColor = Theme::SelectedColor;

				const double vibrateY = vibrateY_whenMoveCursor(timerChangeMenuIndex_.progress1_0());
				drawLineHighlight(itemCenter.y + vibrateY, timerSetBinding_.progress0_1(), timerChangeMenuIndex_.progress1_0());
			}

			// ラベル
			const double labelShiftX = (index == selectedFieldIndex_) ? EaseOutCubic(timerChangeMenuIndex_.progress0_1()) * 2 : 0;
			DrawText(U"px7812", item.text, Arg::leftCenter = Vec2{ Layout::LabelColumnLeftCenterX + labelShiftX, posY }, (selected ? Theme::SelectedLabelColor : Theme::LabelColor));

			// 選択されているゲームコントローラ
			if (item.type == InputSettingField::Type::Controller)
			{
				// 割り当て操作時の振動
				double vibrateX = 0;

				// 割り当て操作時の点滅
				if (not timerSetKey_[index].reachedZero())
				{
					vibrateX = vibrateX_whenBinding(timerSetKey_[index].progress1_0());
					keyColor = valueColor_whenBinding(Theme::ValueColor);
				}

				const uint8 controllerId = getData().actionInput->getControllerId();
				DrawText(U"px7812", Format(U"No.", controllerId), Arg::center = Vec2{Layout::KeyColumnCenterX + vibrateX, posY}, keyColor);
				DrawText(U"px7812", Gamepad(controllerId).isConnected() ? U"OK"_sv : U"(none)"_sv, Arg::center = Vec2{ Layout::ButtonColumnCenterX + vibrateX * 0.3, posY }, (selected ? Theme::SelectedColor : Theme::DisabledValueColor));
			}

			// 設定値（キーボード）
			if (tempKeyBindings_.contains(item.action))
			{
				// 割り当て操作時の振動
				double vibrateX = 0;

				// 割り当て操作時の点滅
				if (not timerSetKey_[index].reachedZero())
				{
					vibrateX = vibrateX_whenBinding(timerSetKey_[index].progress1_0());
					keyColor = valueColor_whenBinding(Theme::ValueColor);
				}

				DrawText(U"px7812", GetInputName(tempKeyBindings_.at(item.action)), Arg::center = Vec2{ Layout::KeyColumnCenterX + vibrateX, posY }, keyColor);
			}

			// 設定値（コントローラ）
			if (tempButtonBindings_.contains(item.action))
			{
				// 割り当て操作時の振動
				double vibrateX = 0;

				// 割り当て操作時の点滅
				if (not timerSetButton_[index].reachedZero())
				{
					vibrateX = vibrateX_whenBinding(timerSetButton_[index].progress1_0());
					buttonColor = valueColor_whenBinding(buttonColor);
				}

				const String buttonName = IsMoveAction(item.action) ? U"-" : GetInputName(tempButtonBindings_.at(item.action));
				DrawText(U"px7812", buttonName, Arg::center = Vec2{ Layout::ButtonColumnCenterX + vibrateX, posY }, buttonColor);
			}
		}
	}

	void InputSettingScene::copySettingToTemporaryData_()
	{
		for (const auto& action : ActionList)
		{
			const auto keys = getData().actionInput->getInputs(action).inputs()
				.filter([](const auto& input) { return input.deviceType() == InputDeviceType::Keyboard; });

			if (not keys.empty())
			{
				tempKeyBindings_[action] = keys.front();
			}

			const auto buttons = getData().actionInput->getInputs(action).inputs()
				.filter([](const auto& input) { return input.deviceType() == InputDeviceType::Gamepad || input.deviceType() == InputDeviceType::XInput; });

			if (not buttons.empty())
			{
				tempButtonBindings_[action] = buttons.front();
			}
		}
	}

	void InputSettingScene::selectNext_()
	{
		selectedFieldIndex_ = (selectedFieldIndex_ + 1) % SettingFieldList.size();
	}

	void InputSettingScene::selectPrevious_()
	{
		selectedFieldIndex_ = (selectedFieldIndex_ + SettingFieldList.size() - 1) % SettingFieldList.size();
	}

	void InputSettingScene::assignKey_(Action action)
	{
		// 現在入力のあるキーのうち、(input.down() == true) のもの
		const auto& keys = Keyboard::GetAllInputs().filter([](const Input& input) { return input.down(); });
		if (keys.empty()) return;

		const auto newKey = keys.front();

		// 同じキーが割り当て済みなら入れ替える
		for (const auto& key : tempKeyBindings_)
		{
			if (key.second.code() == newKey.code())
			{
				tempKeyBindings_[key.first] = tempKeyBindings_[action];

				// 描画用
				startTimerSetKey_(key.first, 0.3s);

				break;
			}
		}

		tempKeyBindings_[action] = newKey;

		// 描画用
		timerSetBinding_.restart(0.2s);
		startTimerSetKey_(action, 0.3s);

		// SE
		PlayAudioOneShot(U"Select1");
	}

	void InputSettingScene::assignButton_(Action action)
	{
		// 移動アクションのゲームコントローラ割り当ては変更しない
		if (IsMoveAction(action)) return;

		// コントローラのボタン（down() == true のもの）
		const auto buttons = GetAllControllerInputs(getData().actionInput->getControllerId());
		if (buttons.empty()) return;

		const auto newButton = buttons.front();

		// 同じボタンが割り当て済みなら入れ替える
		for (const auto& button : tempButtonBindings_)
		{
			// 入れ替え先が移動アクションでないことも確認
			if (button.second == newButton && not IsMoveAction(button.first))
			{
				tempButtonBindings_[button.first] = tempButtonBindings_[action];

				// 描画用
				startTimerSetButton_(button.first, 0.3s);

				break;
			}
		}

		tempButtonBindings_[action] = newButton;

		// 描画用
		timerSetBinding_.restart(0.2s);
		startTimerSetButton_(action, 0.3s);

		// SE
		PlayAudioOneShot(U"Select1");
	}

	void InputSettingScene::saveSetting_()
	{
		const uint8 playerIndex = getData().actionInput->getControllerId();

		getData().actionInput->setInputs(Action::MoveUp, tempKeyBindings_[Action::MoveUp] | DefaultMoveUpButtons(playerIndex));
		getData().actionInput->setInputs(Action::MoveDown, tempKeyBindings_[Action::MoveDown] | DefaultMoveDownButtons(playerIndex));
		getData().actionInput->setInputs(Action::MoveLeft, tempKeyBindings_[Action::MoveLeft] | DefaultMoveLeftButtons(playerIndex));
		getData().actionInput->setInputs(Action::MoveRight, tempKeyBindings_[Action::MoveRight] | DefaultMoveRightButtons(playerIndex));
		getData().actionInput->setInputs(Action::Attack, tempKeyBindings_[Action::Attack] | tempButtonBindings_[Action::Attack]);
		getData().actionInput->setInputs(Action::Cancel, tempKeyBindings_[Action::Cancel] | tempButtonBindings_[Action::Cancel]);
		getData().actionInput->setInputs(Action::Pause, tempKeyBindings_[Action::Pause] | tempButtonBindings_[Action::Pause]);

		getData().actionInput->save(InputSettingFilePath);
	}

	void InputSettingScene::startTimerSetKey_(Action action, const Duration& time)
	{
		for (const auto [index, item] : Indexed(SettingFieldList))
		{
			if (item.action == action)
			{
				timerSetKey_[index].restart(time);
			}
		}
	}

	void InputSettingScene::startTimerSetButton_(Action action, const Duration& time)
	{
		for (const auto [index, item] : Indexed(SettingFieldList))
		{
			if (item.action == action)
			{
				timerSetButton_[index].restart(time);
			}
		}
	}
}

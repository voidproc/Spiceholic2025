#include "OptionScene.h"
#include "Audio/AudioPlay.h"
#include "Audio/AudioVolume.h"
#include "Config/GameConfig.h"
#include "Core/DrawText.h"
#include "Core/DrawSprite.h"
#include "Input/ActionInput.h"
#include "Setting/UserSetting.h"

namespace Spiceholic
{
	namespace
	{
		namespace Layout
		{
			constexpr int LineHeight = 14;
			constexpr double LabelColumnLeftCenterX = 16;
			constexpr double ValueColumnLeftCenterX = SceneSize.x - 96;
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
		}

		struct SettingField
		{
			UserSettingField field;
		};

		constexpr std::array<SettingField, 5> SettingFieldList = { {
			{ UserSettingField::WindowScale },
			{ UserSettingField::SEVolume },
			{ UserSettingField::BGMVolume },
			{ UserSettingField::UseEffect },
			{ UserSettingField::Back },
		} };

		void ChangeSettingValue(UserSetting& setting, const SettingField& targetField, int32 valueDiff1_1)
		{
			switch (targetField.field)
			{
			case UserSettingField::WindowScale:
				setting.get().windowScale = Clamp(setting.get().windowScale + valueDiff1_1 * 1, 1, 8);
				break;

			case UserSettingField::SEVolume:
				setting.get().seVolume = Clamp(setting.get().seVolume + valueDiff1_1 * 5, 0, 100);
				break;

			case UserSettingField::BGMVolume:
				setting.get().bgmVolume = Clamp(setting.get().bgmVolume + valueDiff1_1 * 5, 0, 100);
				break;

			case UserSettingField::UseEffect:
				setting.get().useEffect = not setting.get().useEffect;
				break;
			}
		}

		void DrawSettingField(UserSetting& setting, const SettingField& field, double fieldCenterY, double labelShiftX, double valueShiftX, const ColorF& labelColor, const ColorF& valueColor)
		{
			// ラベル
			DrawText(U"px7812", GetUserSettingFieldName(field.field), Arg::leftCenter = Vec2{ Layout::LabelColumnLeftCenterX + labelShiftX, fieldCenterY }, labelColor);

			// 設定値
			String valueText;
			switch (field.field)
			{
			case UserSettingField::WindowScale:
			{
				const double scale = setting.get().windowScale;
				valueText = U"x{} ({}x{})"_fmt(scale, SceneSize.x * scale, SceneSize.y * scale);
				break;
			}

			case UserSettingField::SEVolume: valueText = Format(setting.get().seVolume, U"％"); break;
			case UserSettingField::BGMVolume: valueText = Format(setting.get().bgmVolume, U"％"); break;

			case UserSettingField::UseEffect: valueText = setting.get().useEffect ? U"On" : U"Off"; break;
			}

			DrawText(U"px7812", valueText, Arg::leftCenter = Vec2{ Layout::ValueColumnLeftCenterX + valueShiftX, fieldCenterY }, valueColor);
		}

		void DrawSelectedLineHighlight(double posY)
		{
			const double alpha = (0.2 + 0.2 * Periodic::Jump0_1(0.75s)) * (0.8 + 0.2 * Periodic::Square0_1(32ms));

			RectF{ Arg::center = SceneRect.center().withY(posY), SizeF{ SceneSize.x, Layout::LineHeight } }.draw(ColorF{ 1, alpha });
		}
	}

	OptionScene::OptionScene(const InitData& init)
		:
		App::Scene{ init },
		menu_{ SettingFieldList.size(), 0.1s },
		timerChangeValue_{},
		valueChangedIndex_{ 0 }
	{
	}

	OptionScene::~OptionScene()
	{
	}

	void OptionScene::update()
	{
		// カーソル移動
		if (getData().actionInput->down(Action::MoveUp))
		{
			menu_.selectPrevious();

			// SE
			PlayAudioOneShot(U"Select2");
		}
		else if (getData().actionInput->down(Action::MoveDown))
		{
			menu_.selectNext();

			// SE
			PlayAudioOneShot(U"Select2");
		}

		// 操作対象
		const auto& selectedField = SettingFieldList[menu_.selectedIndex()];

		// 選択項目に対する決定操作
		if (getData().actionInput->down(Action::Decide))
		{
			if (selectedField.field == UserSettingField::Back)
			{
				// SE
				PlayAudioOneShot(U"Decide1");

				// 設定保存
				getData().userSetting->save(UserSettingFilePath);

				// タイトルシーンへ
				changeScene(U"TitleScene", 0);
				return;
			}
		}

		// 選択項目の値を変更
		if (const int32 valueDiff = 0 + KeyRight.down() - KeyLeft.down();
			valueDiff != 0)
		{
			ChangeSettingValue(*getData().userSetting, selectedField, valueDiff);

			SetAudioVolume(*getData().userSetting);

			// 描画用
			timerChangeValue_.restart(0.2s);
			valueChangedIndex_ = menu_.selectedIndex();

			// SE
			PlayAudioOneShot(U"Select1");
		}
	}

	void OptionScene::draw() const
	{
		// BG
		SceneRect.draw(Theme::BgColor);

		// 背景にキャラを走らせる
		{
			ScopedColorMul2D mul{ 0.5, 0.7 };
			ScopedColorAdd2D add{ 0.5, 0.1, 0.1, 0 };

			const int NX = 6;
			const int NY = 6;
			for (int iY = -1; iY <= NY; ++iY)
			{
				for (int iX = -1; iX <= NX; ++iX)
				{
					const bool evenRow = (iY % 2 == 0);
					const bool evenCol = (iX % 2 == 0);
					const Vec2 charaPos{
						SceneSize.x / NX * (iX + Periodic::Sawtooth0_1(1s) * (evenRow ? 1 : -1)),
						SceneSize.y / NY * (iY + 0.5) };
						//SceneSize.y / NY * (iY + 2 * Periodic::Sawtooth0_1(1s)) };
					DrawSprite(*getData().appSetting, U"PlayerWalkR", 0.5s, not evenRow, charaPos);
				}
			}
		}

		// シーンタイトル
		DrawText(U"px7812", U"Option", Arg::center = SceneRect.center().withY(Layout::LineHeight * 1), Theme::TitleTextColor);

		// 各設定項目を描画
		for (const auto [iField, field] : Indexed(SettingFieldList))
		{
			const double fieldCenterY = Layout::LineHeight * (3.0 + iField + (iField == SettingFieldList.size() - 1 ? 0.5 : 0));
			const bool selected = (iField == menu_.selectedIndex());

			// 選択行
			if (selected)
			{
				DrawSelectedLineHighlight(fieldCenterY + MenuMoveCursorVibrateVertical(menu_).y);
			}

			const double labelShiftX = selected ? 4 * EaseOutCubic(menu_.moveCursorTimer().progress0_1()) : 0;
			const ColorF labelColor = selected ? Theme::SelectedLabelColor : Theme::LabelColor;
			const bool valueChanged = (iField == valueChangedIndex_) && timerChangeValue_.isRunning();
			const double valueShiftX = valueChanged ? 2 * Periodic::Sine1_1(0.01s) : 0;
			const ColorF valueColor = valueChanged ? Theme::ValueColor.lerp(Palette::Black, 0.5).lerp(Theme::ValueChangeColor, Periodic::Square0_1(0.034s)) : (selected ? Theme::SelectedColor : Theme::ValueColor);
			DrawSettingField(*getData().userSetting, field, fieldCenterY, labelShiftX, valueShiftX, labelColor, valueColor);
		}
	}
}

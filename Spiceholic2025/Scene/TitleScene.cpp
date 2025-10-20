#include "TitleScene.h"
#include "Config/GameConfig.h"
#include "Core/DrawText.h"
#include "Input/ActionInput.h"

namespace Spiceholic
{
	namespace
	{
		namespace Theme
		{
			constexpr ColorF BgColor = Palette::Indianred.lerp(Palette::Darkred, 0.3);
		}

		enum class TitleMenuItemType
		{
			Start,
			Option,
			InputSetting,
			Quit,
		};

		struct TitleMenuItem
		{
			TitleMenuItemType type;
			StringView text;
		};

		constexpr std::array<TitleMenuItem, 4> TitleMenuItemList = { {
			{ TitleMenuItemType::Start, U"Game Start"_sv },
			{ TitleMenuItemType::Option, U"Option"_sv },
			{ TitleMenuItemType::InputSetting, U"Input Setting"_sv },
			{ TitleMenuItemType::Quit, U"Quit"_sv },
		} };
	}

	TitleScene::TitleScene(const InitData& init)
		:
		CustomScene{ init },
		timerSceneChange_{ 1.0s, StartImmediately::No },
		selectedMenuIndex_{ 0 }
	{
	}

	TitleScene::~TitleScene()
	{
	}

	void TitleScene::update()
	{
		// カーソル移動
		// GameStart決定後は行わない
		if (not timerSceneChange_.isRunning())
		{
			if (getData().actionInput->down(Action::MoveLeft))
			{
				selectPrevious_();

				// 描画用
				timerMoveCursorLeft_.restart(0.1s);
			}
			else if (getData().actionInput->down(Action::MoveRight))
			{
				selectNext_();

				// 描画用
				timerMoveCursorRight_.restart(0.1s);
			}
		}

		// 選択項目
		const auto& selected = TitleMenuItemList[selectedMenuIndex_];

		// 選択項目に対する決定操作
		if (getData().actionInput->down(Action::Decide))
		{
			if (selected.type == TitleMenuItemType::Start &&
				not timerSceneChange_.isRunning())
			{
				// メインシーンへ
				timerSceneChange_.start();
			}
			else if (selected.type == TitleMenuItemType::Option)
			{
				// オプションシーンへ
				changeScene(U"OptionScene", 0);
			}
			else if (selected.type == TitleMenuItemType::InputSetting)
			{
				// 入力設定シーンへ
				changeScene(U"InputSettingScene", 0);
			}
			else if (selected.type == TitleMenuItemType::Quit)
			{
				// アプリ終了
				System::Exit();
			}
		}

		if (timerSceneChange_.reachedZero())
		{
			changeScene(U"MainScene", 0);
		}
	}

	void TitleScene::draw() const
	{
		// BG
		SceneRect.draw(Theme::BgColor);

		// タイトル
		DrawText(U"px7812", U"Blazing Spiceholics", Arg::center = SceneRect.center().withY(64), Palette::White);

		// メニュー
		const Vec2 menuCenter = SceneRect.center() + Vec2{ 0, 16 };
		const double vibrateX_l = timerMoveCursorLeft_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
		const double vibrateX_r = timerMoveCursorRight_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
		DrawText(U"px7812", TitleMenuItemList[selectedMenuIndex_].text, Arg::center = menuCenter + Vec2{ Max(vibrateX_l, vibrateX_r), 0 }, Palette::White);

		// メニュー矢印
		{
			const double color = 0.7 + 0.3 * Periodic::Square0_1(0.5s);
			TextureAsset(U"WhiteArrow")(0, 0, 16).drawAt(menuCenter - Vec2{ 75 + vibrateX_l, 0 }, ColorF(color));
			TextureAsset(U"WhiteArrow")(16, 0, 16).drawAt(menuCenter + Vec2{ 75 + vibrateX_r, 0 }, ColorF(color));
		}

		// フェード
		{
			const double alpha = Saturate(2.5 * timerSceneChange_.progress0_1());
			SceneRect.draw(ColorF{ Palette::Whitesmoke, EaseOutQuad(alpha) });
		}
	}

	void TitleScene::selectNext_()
	{
		selectedMenuIndex_ = (selectedMenuIndex_ + 1) % TitleMenuItemList.size();
	}

	void TitleScene::selectPrevious_()
	{
		selectedMenuIndex_ = (selectedMenuIndex_ + TitleMenuItemList.size() - 1) % TitleMenuItemList.size();
	}
}

#include "TitleScene.h"
#include "Config/GameConfig.h"
#include "Core/DrawText.h"

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
			Quit,
		};

		struct TitleMenuItem
		{
			TitleMenuItemType type;
			StringView text;
		};

		constexpr std::array<TitleMenuItem, 3> TitleMenuItemList = { {
			{ TitleMenuItemType::Start, U"Start"_sv },
			{ TitleMenuItemType::Option, U"Option"_sv },
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
			if (KeyLeft.down())
			{
				selectPrevious_();
			}
			else if (KeyRight.down())
			{
				selectNext_();
			}
		}

		// 選択項目
		const auto& selected = TitleMenuItemList[selectedMenuIndex_];

		// 選択項目に対する決定操作
		if (KeyZ.down())
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
			else if (selected.type == TitleMenuItemType::Quit)
			{
				// アプリ終了
				System::Exit();
			}
		}

		// メインシーン移行タイマー
		if (KeyEnter.down() && not timerSceneChange_.isRunning())
		{
			timerSceneChange_.start();
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
		//FontAsset(U"px7812")(U"Blazing Spiceholics").drawAt(SceneCenter + Vec2::One(), ColorF{ 0, 0.3 });
		//FontAsset(U"px7812")(U"Blazing Spiceholics").drawAt(SceneCenter, Palette::White.lerp(Palette::Gray, 0.3 * Periodic::Sine0_1(0.3s)));

		// メニュー
		const Vec2 menuCenter = SceneRect.center() + Vec2{ 0, 16 };
		DrawText(U"px7812", TitleMenuItemList[selectedMenuIndex_].text, Arg::center = menuCenter, Palette::White);

		// メニュー矢印
		{
			const double alpha = Periodic::Square0_1(0.5s);
			TextureAsset(U"WhiteArrow")(0, 0, 16).drawAt(menuCenter - Vec2{ 75, 0 }, AlphaF(Saturate(alpha)));
			TextureAsset(U"WhiteArrow")(16, 0, 16).drawAt(menuCenter + Vec2{ 75, 0 }, AlphaF(Saturate(alpha)));
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

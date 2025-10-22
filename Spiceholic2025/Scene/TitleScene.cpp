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
		time_{ StartImmediately::Yes },
		timerSceneChange_{ 1.0s, StartImmediately::No },
		selectedMenuIndex_{ 0 },
		randomCharaTexName_{ Random() < 0.8 ? U"DragonGirl" : U"DragonGirl2" }
	{
		if (getData().titleCharacterShown)
		{
			time_.set(2.7s);
		}
	}

	TitleScene::~TitleScene()
	{
	}

	void TitleScene::update()
	{
		if (time_ < 2.7s)
		{
			if (getData().actionInput->down(Action::Decide))
			{
				time_.set(2.7s);
			}

			return;
		}

		getData().titleCharacterShown = true;

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
		{
			const double alpha = Saturate((time_.sF() - 0.9) / 0.4);
			SceneRect.draw(Palette::White.lerp(Theme::BgColor, alpha));
			RectF{ Arg::topCenter = SceneRect.topCenter(), SizeF{ SceneSize.x, SceneSize.y / 3 } }.draw(Arg::top = ColorF{ Palette::Darkred, 0.3 * alpha }, Arg::bottom = ColorF{ Palette::Darkred, 0 });
			RectF{ Arg::bottomCenter = SceneRect.bottomCenter(), SizeF{ SceneSize.x, SceneSize.y / 3 } }.draw(Arg::top = ColorF{ Palette::Darkred, 0 }, Arg::bottom = ColorF{ Palette::Darkred, 0.3 * alpha });
		}

		// キャラクター
		{
			const double t = Saturate(time_.sF() / 1.1);
			const double t2 = 1.0 - 0.7 * Saturate((time_.sF() - 1.5) / 0.45);
			{
				ScopedColorMul2D mul{ 1, t2 };

				const double tMoveCursor = Saturate(1 - (timerMoveCursorLeft_.progress1_0() + timerMoveCursorRight_.progress1_0()));
				const double jump = Periodic::Jump0_1(0.1s, tMoveCursor * 0.1);
				const Vec2 pos = SceneRect.bottomCenter() + Vec2{ -40 + 300 * (1 - EaseOutCubic(t)), -5 * Periodic::Sine1_1(6.0s) - 64 } + Vec2{ 0, -4 * jump };

				TextureAsset(randomCharaTexName_).scaled(1 + 0.03 * jump, 1 + 0.05 * jump).drawAt(pos + Vec2::One() * 4, ColorF{ Palette::Darkred, 0.3 });
				TextureAsset(randomCharaTexName_).scaled(1 + 0.03 * jump, 1 + 0.05 * jump).drawAt(pos);
			}

			const double t3 = Saturate((time_.sF() - 0.9) / 0.8);
			const double t4 = Saturate((time_.sF() - 0.9) / 0.30);
			const ColorF barColor{ 1, 1 - t4 };
			RectF{ 0, 0, SceneSize.x, (130 - 16) * (1 - EaseOutCubic(t3)) }.draw(barColor);
			RectF{ 0, 130 + 16 + (130 - 16) * EaseOutCubic(t3), SceneSize.x, SceneSize.y - (130 + 16) }.draw(barColor);
		}

		// タイトル
		{
			Transformer2D scale{ Mat3x2::Scale(1, 2) };
			const double alpha = Saturate((time_.sF() - 1.5) / 0.5);
			const Vec2 center = SceneRect.center().withY(20);
			RectF{ Arg::center = center + Vec2{ 0, 1 }, SizeF{SceneSize.x, 14} }.draw(ColorF{ Palette::Darkred, 0.8 * alpha });
			const ColorF textColor = Palette::Yellow.lerp(Palette::Red.lerp(Palette::Lime.lerp(Palette::White, Periodic::Sine0_1(0.3s)), Periodic::Sine0_1(0.4s)), Periodic::Sine0_1(0.45s));
			DrawText(U"NotJamSig21", U"Blazing Spiceholics", Arg::center = center, ColorF{ textColor, alpha }, ColorF{ 0, 0.5 * alpha });
		}

		// メニュー
		if (time_ > 2.7s)
		{
			const Vec2 menuCenter = SceneRect.center() + Vec2{ 0, 16 };
			const double vibrateX_l = timerMoveCursorLeft_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
			const double vibrateX_r = timerMoveCursorRight_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
			RectF{ Arg::center = menuCenter, SizeF{ SceneSize.x, 20 } }.draw(ColorF{ Palette::Darkred, 0.5 });
			const ColorF color = Palette::White.lerp(Palette::Darkred, 0.2 * Periodic::Square0_1(0.5s));
			DrawText(U"px7812", TitleMenuItemList[selectedMenuIndex_].text, Arg::center = menuCenter + Vec2{ Max(vibrateX_l, vibrateX_r), 0 }, color);

			// メニュー矢印
			{
				TextureAsset(U"WhiteArrow")(0, 0, 16).drawAt(menuCenter - Vec2{ 75 + vibrateX_l, 0 }, color);
				TextureAsset(U"WhiteArrow")(16, 0, 16).drawAt(menuCenter + Vec2{ 75 + vibrateX_r, 0 }, color);
			}
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

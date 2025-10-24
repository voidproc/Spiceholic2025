#include "TitleScene.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Input/ActionInput.h"

namespace Spiceholic
{
	namespace
	{
		constexpr Duration TimeEnableMenu = 3.5s;

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
			StringView desc;
		};

		constexpr std::array<TitleMenuItem, 4> TitleMenuItemList = { {
			{ TitleMenuItemType::Start, U"Game Start"_sv, U"ゲームをはじめます"_sv },
			{ TitleMenuItemType::Option, U"Option"_sv, U"画面や音量などの設定をします"_sv  },
			{ TitleMenuItemType::InputSetting, U"Input Setting"_sv, U"キーボード・ゲームコントローラの設定をします"_sv  },
			{ TitleMenuItemType::Quit, U"Quit"_sv, U"ゲームを終了します"_sv  },
		} };
	}

	TitleScene::TitleScene(const InitData& init)
		:
		CustomScene{ init },
		time_{ StartImmediately::Yes },
		timeDecide_{ StartImmediately::No },
		//timerSceneChange_{ 0.7s, StartImmediately::No },
		selectedMenuIndex_{ 0 },
		randomCharaTexName_{ U"DragonGirl" },
		timeDescText_{ StartImmediately::Yes },
		timerFadeIn_{ 0.50s, StartImmediately::Yes }
	{
		if (getData().titleCharacterShown)
		{
			time_.set(TimeEnableMenu);
			randomCharaTexName_ = Random() < 0.8 ? U"DragonGirl" : U"DragonGirl2";
		}
	}

	TitleScene::~TitleScene()
	{
	}

	void TitleScene::update()
	{
		if (time_ < TimeEnableMenu)
		{
			if (getData().actionInput->down(Action::Decide))
			{
				time_.set(TimeEnableMenu);
			}

			return;
		}

		getData().titleCharacterShown = true;

		// カーソル移動、項目決定
		// 項目決定後は行わない
		if (not timeDecide_.isRunning())
		{
			// カーソル移動
			if (getData().actionInput->down(Action::MoveLeft))
			{
				selectPrevious_();

				// 描画用
				timerMoveCursorLeft_.restart(0.1s);
				timeDescText_.restart();
			}
			else if (getData().actionInput->down(Action::MoveRight))
			{
				selectNext_();

				// 描画用
				timerMoveCursorRight_.restart(0.1s);
				timeDescText_.restart();
			}

			// 選択項目に対する決定操作
			if (getData().actionInput->down(Action::Decide))
			{
				timeDecide_.start();
			}
		}

		if (timeDecide_ > 0.95s)
		{
			if (const auto& selected = TitleMenuItemList[selectedMenuIndex_];
				selected.type == TitleMenuItemType::Start)
			{
				// メインシーンへ
				changeScene(U"MainScene", 0);
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
	}

	void TitleScene::draw() const
	{
		// BG
		{
			// 背景色
			const ColorF bgColor = Palette::Indianred.lerp(Palette::Darkred, 0.3);
			// 背景色・背景グラデアルファ (1.3～2.1s)
			const double t = Saturate((time_.sF() - 1.3) / 0.8);
			SceneRect.draw(Palette::White.lerp(bgColor, t));
			// 背景グラデ
			const ColorF bgGradA0{ Palette::Darkred, 0.0 };
			const ColorF bgGradA1{ Palette::Darkred, 0.3 * t };
			const SizeF gradRectSize{ SceneSize.x, SceneSize.y / 3 };
			RectF{ Arg::topCenter = SceneRect.topCenter(), gradRectSize}.draw(Arg::top = bgGradA1, Arg::bottom = bgGradA0);
			RectF{ Arg::bottomCenter = SceneRect.bottomCenter(), gradRectSize }.draw(Arg::top = bgGradA0, Arg::bottom = bgGradA1);
		}

		// キャラクター
		{
			// おまけのエフェクト（メニュー選択時の反応）
			const double tMoveCursor = Saturate(1 - (timerMoveCursorLeft_.progress1_0() + timerMoveCursorRight_.progress1_0()));
			const Vec2 jump{ 0, Periodic::Jump0_1(0.1s, tMoveCursor * 0.1) };
			const Vec2 scale{ 1 + 0.03 * jump.y, 1 + 0.05 * jump.y };
			// 縦揺れ (3.0s～)
			const Vec2 wave{ 0, -5 * Periodic::Sine1_1(6.0s) * Saturate((time_.sF() - 3.0) / 1.0) };
			// 横移動 (0.0～1.1s, 0.9～2.0s)
			const double t1a = Saturate(time_.sF() / 1.1);
			const double t1b = Saturate((time_.sF() - 0.9) / 1.1);
			const Vec2 basePos = SceneRect.bottomCenter() + Vec2{ -40, -64 };
			const Vec2 moveX{ 300 * (1 - (0.85 * EaseOutCubic(t1a) + 0.15 * EaseOutBack(t1b))), 0 };
			const Vec2 pos = basePos + moveX + wave - jump * 4;
			// 登場時拡大～縮小 (0.0～0.85s)
			const Vec2 scale2 = Vec2::One() * 0.2 * (1 - EaseInOutQuad(Saturate(time_.sF() / 0.85)));
			// アルファ (1.8～2.3s)
			const double alphaMul = 1.0 - 0.65 * Saturate((time_.sF() - 1.8) / 0.5);
			ScopedColorMul2D mul{ 1, alphaMul };

			TextureAsset(randomCharaTexName_).scaled(scale + scale2).drawAt(pos + Vec2::One() * 4, ColorF{ Palette::Darkred, 0.3 });
			TextureAsset(randomCharaTexName_).scaled(scale + scale2).drawAt(pos);
		}

		// キャラ登場時 目線用の帯
		{
			// 目線幅 (0.0～0.5s)
			const double barHeight = 22 * EaseOutCirc(Saturate(time_.sF() / 0.5));
			// 白矩形アルファ 1.2～1.5s)
			const double barAlpha = 1.0 - Saturate((time_.sF() - 1.2) / 0.3);
			const ColorF barColor{ 1, barAlpha };
			const double barCenterY = 127;
			// 上下に開放 (0.95～1.75s)
			const double t3 = Saturate((time_.sF() - 0.95) / 0.8);
			RectF{ 0, 0, SceneSize.x, (barCenterY - barHeight / 2) * (1 - EaseOutCubic(t3)) }.draw(barColor);
			RectF{ 0, (barCenterY + barHeight / 2) + (barCenterY - barHeight / 2) * EaseOutCubic(t3), SceneSize.x, SceneSize.y - (barCenterY + barHeight / 2) }.draw(barColor);
		}

		// タイトル
		{
			Transformer2D scale{ Mat3x2::Scale(1, 2) };

			// タイトルアルファ (2.0～2.6s)
			const double alphaMul = Saturate((time_.sF() - 2.0) / 0.6);
			ScopedColorMul2D mul{ 1, alphaMul };

			const Vec2 center = SceneRect.center().withY(20);
			RectF{ Arg::center = center + Vec2{ 0, 1 }, SizeF{ SceneSize.x, 14 } }.draw(ColorF{ Palette::Darkred, 0.8 });
			const ColorF textColor = Palette::Yellow.lerp(Palette::Red.lerp(Palette::Lime.lerp(Palette::White, Periodic::Sine0_1(0.3s)), 0.7 * Periodic::Sine0_1(0.4s)), 0.8 * Periodic::Sine0_1(0.45s));
			DrawText(U"NotJamSig21", U"Blazing Spiceholics", Arg::center = center, ColorF{ textColor }, ColorF{ 0, 0.5 });
		}

		// メニュー
		if (time_ > TimeEnableMenu)
		{
			const Vec2 menuCenter = SceneRect.center() + Vec2{ 0, 16 };
			const double vibrateX_l = timerMoveCursorLeft_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
			const double vibrateX_r = timerMoveCursorRight_.isRunning() ? 2 * Periodic::Sine1_1(0.01s) : 0;
			RectF{ Arg::center = menuCenter, SizeF{ SceneSize.x, 20 } }.draw(ColorF{ Palette::Darkred, 0.5 });
			const ColorF color = (not timeDecide_.isRunning()) ?
				Palette::White.lerp(Palette::Indianred, 0.4 * Periodic::Square0_1(0.5s)) :
				LightYellow.lerp(Palette::Darkred, 0.8 * Periodic::Square0_1(0.15s));
			DrawText(U"px7812", TitleMenuItemList[selectedMenuIndex_].text, Arg::center = menuCenter + Vec2{ Max(vibrateX_l, vibrateX_r), 0 }, color);

			// メニュー矢印
			if (not timeDecide_.isRunning())
			{
				TextureAsset(U"WhiteArrow")(0, 0, 16).drawAt(menuCenter - Vec2{ 75 + vibrateX_l, 0 }, color);
				TextureAsset(U"WhiteArrow")(16, 0, 16).drawAt(menuCenter + Vec2{ 75 + vibrateX_r, 0 }, color);
			}

			// 説明
			StringView desc = TitleMenuItemList[selectedMenuIndex_].desc;
			DrawText(U"px7812", desc.substr(0, Min(999.0, timeDescText_.sF() * 40.0)), Arg::center = SceneRect.center().withY(SceneSize.y - 24), ColorF{ 1, 0.8 });
		}

		// フェードアウト
		{
			const double alpha = Saturate((timeDecide_.sF() - 0.65) / 0.30);
			SceneRect.draw(ColorF{ Palette::Whitesmoke, EaseOutQuad(alpha) });
		}

		// フェードイン
		{
			const double alpha = timerFadeIn_.progress1_0();
			SceneRect.draw(ColorF{ Palette::Whitesmoke, EaseInQuad(alpha) });
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

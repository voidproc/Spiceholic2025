#include "RecordsScene.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Core/DrawSprite.h"
#include "Core/StageRecord.h"
#include "Input/ActionInput.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		constexpr ColorF BgColor = Palette::Indianred.lerp(Palette::Darkred, 0.5);
		constexpr ColorF TitleTextColor = Palette::Whitesmoke;
		constexpr ColorF ClearColor = Palette::Lightcoral.lerp(Palette::Yellow, 0.5);
		constexpr ColorF LabelColor = Palette::Lightcoral.lerp(Palette::White, 0.3);
		constexpr ColorF PartialClearColor = Palette::Cyan.lerp(Palette::Gray, 0.6);
		//constexpr ColorF ValueChangeColor = Palette::Yellow;
		//constexpr ColorF SelectedLabelColor = Palette::Yellow.lerp(Palette::White, 0.5);
		//constexpr ColorF SelectedColor = Palette::Whitesmoke;
	}

	RecordsScene::RecordsScene(const InitData& init)
		:
		CustomScene{ init },
		time_{ StartImmediately::Yes }
	{
	}

	RecordsScene::~RecordsScene()
	{
	}

	void RecordsScene::update()
	{
		if (getData().actionInput->down(Action::Decide))
		{
			changeScene(U"TitleScene", 0s);

			// SE
			PlayAudioOneShot(U"Decide1");
		}

	}

	void RecordsScene::draw() const
	{
		// BG
		SceneRect.draw(BgColor);

		constexpr double LineHeight = 14;

		// 背景にキャラを走らせる
		{
			ScopedColorMul2D mul{ 0.5, 0.5 };
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
					DrawSprite(*getData().appSetting, U"EnemyChick", 0.3s, not evenRow, charaPos);
				}
			}
		}

		// シーンタイトル
		DrawText(U"px7812", U"Records", Arg::center = SceneRect.center().withY(LineHeight * 0.5 + 1), TitleTextColor);

		for (const auto [index, stage] : Indexed(getData().appSetting->get().stageList))
		{
			const double posY = LineHeight * (2 + index) - 2;

			// ラベル
			const Vec2 labelPos{ SceneCenter.x - 64 - 24, posY };
			DrawText(U"px7812m", U"Stage{}"_fmt(stage), Arg::leftCenter = labelPos, LabelColor);

			// レコード
			const Vec2 valuePos{ SceneCenter.x - 26, posY };
			if (getData().currentRecords.contains(stage))
			{
				const auto& rec = getData().currentRecords[stage];
				int sec = static_cast<int>(Math::Floor(rec.timeSec));
				DrawText(U"px7812m", U"{:-3d}:{:02d}   {}"_fmt(sec / 60, sec % 60, GetClearTypeName(rec.clearType)), Arg::leftCenter = valuePos, rec.clearType == ClearType::Cleared ? ClearColor : PartialClearColor);
			}
			else
			{
				{
					DrawText(U"px7812m", U" --:--   No data", Arg::leftCenter = valuePos, Palette::Indianred);
				}
			}

		}
	}
}

#include "RecordsScene.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Core/StageRecord.h"
#include "Input/ActionInput.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		constexpr ColorF BgColor = Palette::Indianred.lerp(Palette::Darkred, 0.5);
		constexpr ColorF TitleTextColor = Palette::Whitesmoke;
		constexpr ColorF LabelColor = Palette::Lightcoral.lerp(Palette::Yellow, 0.3);
		constexpr ColorF ValueColor = Palette::Lightcoral.lerp(Palette::White, 0.3);
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

		// シーンタイトル
		DrawText(U"px7812", U"Records", Arg::center = SceneRect.center().withY(LineHeight * 0.5), TitleTextColor);

		for (const auto [index, stage] : Indexed(getData().appSetting->get().stageList))
		{
			// ラベル
			const Vec2 labelCenter{ SceneCenter.x - 64 - 24, LineHeight * (2 + index) };
			DrawText(U"px7812m", U"Stage{}"_fmt(stage), Arg::leftCenter = labelCenter, ValueColor);

			// レコード
			const Vec2 valueCenter{ SceneCenter.x + 64, LineHeight * (2 + index) };
			Optional<int> s;
			if (getData().currentRecords.contains(stage))
			{
				s = static_cast<int>(Math::Floor(getData().currentRecords[stage].timeSec));
				DrawText(U"px7812m", U"{:-2d}:{:02d}"_fmt(*s / 60, *s % 60), Arg::center = valueCenter, LabelColor);
			}
			else
			{
				DrawText(U"px7812m", U"(no data)", Arg::center = valueCenter, LabelColor);
			}

		}
	}
}

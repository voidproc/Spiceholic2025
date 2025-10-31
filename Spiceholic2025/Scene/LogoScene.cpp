#include "LogoScene.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Input/ActionInput.h"

namespace Spiceholic
{
	namespace
	{
		//constexpr ColorF BgColor = Palette::Indianred.lerp(Palette::Darkred, 0.5);
		//constexpr ColorF TitleTextColor = Palette::Whitesmoke;
	}

	LogoScene::LogoScene(const InitData& init)
		:
		CustomScene{ init },
		time_{ StartImmediately::Yes }
	{
	}

	LogoScene::~LogoScene()
	{
	}

	void LogoScene::update()
	{
		if (getData().actionInput->down(Action::Decide) ||
			time_ > 4.5s)
		{
			changeScene(U"TitleScene", 0s);
		}
	}

	void LogoScene::draw() const
	{
		// BG
		SceneRect.draw(Palette::Whitesmoke);

		// シーンタイトル
		const double rectHeight = SceneSize.y - (SceneSize.y - 32) * EaseInOutQuad(Saturate(time_.sF() / 0.7));
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, rectHeight } }.draw(Palette::Royalblue);

		// Powered by Siv3D
		if (time_ > 0.75s)
		{
			const double alpha = EaseOutQuad(Saturate((time_.sF() - 0.75) / 0.1));
			DrawText(U"px7812", U"Powered by Siv3D", Arg::center = SceneRect.center(), ColorF{ Palette::Whitesmoke, alpha });
		}

		// フェードアウト (3.0～4.0s)
		if (time_ > 3.0s)
		{
			const double alpha = EaseOutSine(Saturate((time_.sF() - 3.0) / 1.0));
			SceneRect.draw(ColorF{ Palette::Whitesmoke, alpha });
		}
	}
}

#include "TitleScene.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
	}

	TitleScene::TitleScene(const InitData& init)
		:
		CustomScene{ init },
		timerSceneChange_{ 1.0s, StartImmediately::No }
	{
	}

	TitleScene::~TitleScene()
	{
	}

	void TitleScene::update()
	{
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
		SceneRect.draw(Palette::Indianred);

		// タイトル
		FontAsset(U"px7812")(U"Blazing Spiceholics").drawAt(SceneCenter + Vec2::One(), ColorF{ 0, 0.3 });
		FontAsset(U"px7812")(U"Blazing Spiceholics").drawAt(SceneCenter, Palette::White.lerp(Palette::Gray, 0.3 * Periodic::Sine0_1(0.3s)));

		// フェード
		const double alpha = Saturate(2.5 * timerSceneChange_.progress0_1());
		SceneRect.draw(ColorF{ Palette::Whitesmoke, EaseOutQuad(alpha) });

	}
}

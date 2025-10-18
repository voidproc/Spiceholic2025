#include "MainScene.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	MainScene::MainScene(const InitData& init)
		:
		CustomScene{ init }
	{
	}

	MainScene::~MainScene()
	{
	}

	void MainScene::update()
	{
	}

	void MainScene::draw() const
	{
		// BG
		SceneRect.draw(Palette::Darkolivegreen.lerp(Palette::Cyan, 0.5));//

		// プレイヤー
		int charaAnimFrame = 4 + Periodic::Square0_1(0.9s);
		TextureAsset(U"Chara")(charaAnimFrame * 48, 0, 48).drawAt(SceneCenter);

		// HUD
		{
			// BG (0, 0), (0, 176)
			Rect{ 0, 0, Size{ SceneSize.x, 16 } }.draw(ColorF{ 0, 0.5 });
			Rect{ 0, 176, Size{ SceneSize.x, 16 } }.draw(ColorF{ 0, 0.5 });

			// ゲージ枠、ゲージ
			const Vec2 gaugePos{ 60, 174 };
			TextureAsset(U"Gauge").draw(gaugePos + Vec2{ 21, 7 });
			TextureAsset(U"GaugeFrame").draw(gaugePos);
		}
	}
}

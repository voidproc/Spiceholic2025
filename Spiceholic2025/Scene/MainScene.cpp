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

		// プレイヤー（暫定）
		int charaAnimFrame = 4 + Periodic::Square0_1(0.9s);
		TextureAsset(U"Chara")(charaAnimFrame * 48, 0, 48).drawAt(SceneCenter);
	}

}

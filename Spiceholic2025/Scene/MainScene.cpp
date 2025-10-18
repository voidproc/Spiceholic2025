#include "MainScene.h"

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
		TextureAsset(U"Chara")(0, 0, 48).scaled(3).drawAt(Scene::CenterF());
	}

}

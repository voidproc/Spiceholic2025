#include "MainScene.h"
#include "Actor/Player.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
		void UpdateActorPos(Actor& actor)
		{
			actor.applyMoveX(1.0);
			actor.applyMoveY(1.0);

			actor.confirmPosition();
		}

	}

	MainScene::MainScene(const InitData& init)
		:
		CustomScene{ init }
	{
		// プレイヤーを初期化
		getData().player = std::make_unique<Player>(SceneCenter);
	}

	MainScene::~MainScene()
	{
	}

	void MainScene::update()
	{
		auto& player = *getData().player;

		// プレイヤーを更新
		player.update();

		// アクターの位置を更新
		UpdateActorPos(player);

	}

	void MainScene::draw() const
	{
		// BG
		SceneRect.draw(Palette::Darkolivegreen.lerp(Palette::Cyan, 0.5));//

		// プレイヤー
		getData().player->draw();

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

#include "MainScene.h"
#include "Actor/Block.h"
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

		// ブロックを適用に生成
		const Grid<int> stageMap = {
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 1, 2, 1, 0, 0, 0, 1, 2, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 1, 2, 1, 0, 0, 0, 1, 2, 1, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		};

		for (int32 iY = 0; iY < stageMap.height(); ++iY)
		{
			for (int32 iX = 0; iX < stageMap.width(); ++iX)
			{
				ActorType type = ActorType::None;
				if (const int num = stageMap[iY][iX];
					num == 1)
				{
					type = ActorType::BlockSteel;
				}
				else if (num == 2)
				{
					type = ActorType::BlockCanBreak;
				}

				if (type != ActorType::None)
				{
					const Vec2 pos = Vec2{ 0.5 + iX, 1.5 + iY } * 16;
					getData().blocks.push_back(std::make_unique<Block>(pos, type));
				}
			}
		}
	}

	MainScene::~MainScene()
	{
	}

	void MainScene::update()
	{
		auto& player = *getData().player;

		// プレイヤーを更新
		player.update();

		// ブロックを更新
		for (auto& block : getData().blocks)
		{
			block->update();
		}

		// アクターの位置を更新
		UpdateActorPos(player);

	}

	void MainScene::draw() const
	{
		// BG
		SceneRect.draw(Palette::Darkolivegreen.lerp(Palette::Cyan, 0.5));//

		// プレイヤー
		getData().player->draw();

		// ブロック
		for (const auto& block : getData().blocks)
		{
			block->draw();
		}

		// HUD
		{
			// BG (0, 0), (0, 176)
			Rect{ 0, 0, Size{ SceneSize.x, 16 } }.draw(ColorF{ 0, 0.5 });
			Rect{ 0, 176, Size{ SceneSize.x, 16 } }.draw(ColorF{ 0, 0.5 });

			// ゲージ枠、ゲージ
			const Vec2 gaugePos{ 60, 174 };
			TextureAsset(U"Gauge").draw(gaugePos + Vec2{ 21, 7 }, ColorF{ 1 - 0.08 * Periodic::Sine0_1(0.3s)});
			TextureAsset(U"GaugeFrame").draw(gaugePos);
		}
	}
}

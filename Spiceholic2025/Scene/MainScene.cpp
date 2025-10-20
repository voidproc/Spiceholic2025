#include "MainScene.h"
#include "Actor/Block.h"
#include "Actor/Enemy.h"
#include "Actor/Item.h"
#include "Actor/Player.h"
#include "Config/GameConfig.h"
#include "Input/ActionInput.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
	namespace
	{
		// だいたい a <= b かどうか
		bool IsLessOrEqualApprox(double a, double b)
		{
			return a < b || AbsDiff(a, b) < 1e-6;
		}

		// ActorSpawnInfoをもとにアクターを生成する
		void SpawnActor(const ActorSpawnInfo& spawn, GameData& gameData)
		{
			switch (spawn.type)
			{
			case ActorType::BlockSteel:
			case ActorType::BlockCanBreak:
			case ActorType::BlockGiftbox:
				gameData.blocks.push_back(std::make_unique<Block>(spawn.position, spawn.type, gameData));
				break;

			case ActorType::ItemChilipepper:
			case ActorType::ItemKey:
				gameData.actors.push_back(std::make_unique<Item>(spawn.position, spawn.type, gameData));
				break;

			case ActorType::EnemyChick:
				gameData.actors.push_back(std::make_unique<EnemyChick>(spawn.position, gameData));
				break;

			default:
				break;
			}
		}

		// 指定した時刻で生成する必要のあるアクターを生成
		void SpawnActors(double currentTime, StageData& stageData, GameData& gameData)
		{
			// 生成
			for (const auto& spawn : stageData.actorSpawns)
			{
				if (not IsLessOrEqualApprox(spawn.time, currentTime)) continue;

				SpawnActor(spawn, gameData);
			}

			// 生成済みのデータを削除しておく
			stageData.actorSpawns.remove_if([&currentTime](const auto& spawn) { return IsLessOrEqualApprox(spawn.time, currentTime); });
		}

		bool IsColliding(Actor& actor, const Array<std::unique_ptr<Block>>& blocks)
		{
			for (const auto& block : blocks)
			{
				if (actor.isCollidingWith(*block))
				{
					return true;
				}
			}

			return false;
		}

		// アクターの位置がブロックに重ならないよう調整
		void UpdateActorPos(Actor& actor, const Array<std::unique_ptr<Block>>& blocks)
		{
			constexpr int N = 4;

			// まずX位置を調整
			for (int iN = N; iN >= 1; --iN)
			{
				const double ratio = 1.0 * iN / N;
				actor.applyMoveX(ratio);

				// アクターとブロックとの衝突確認
				// もし衝突していたらX位置を戻す
				if (IsColliding(actor, blocks))
				{
					actor.revertMoveX();
				}
				else
				{
					break;
				}
			}

			// まだ衝突していたらしょうがないから1pxずつ戻す
			const auto origMoveAmount = actor.getMoveAmount();
			double dirX = Sign(origMoveAmount.x);
			for (int iN = 1; iN <= 4; ++iN)
			{
				if (IsColliding(actor, blocks))
				{
					actor.setMoveAmount(origMoveAmount - Vec2{ dirX, 0 });
					actor.applyMoveX();
				}
				else
				{
					break;
				}
			}

			// Y位置を反映
			for (int iN = N; iN >= 1; --iN)
			{
				const double ratio = 1.0 * iN / N;
				actor.applyMoveY(ratio);

				// アクターとブロックとの衝突確認
				// もし衝突していたらX位置を戻す
				if (IsColliding(actor, blocks))
				{
					actor.revertMoveY();
				}
				else
				{
					break;
				}
			}

			// まだ衝突していたらしょうがないから1pxずつ戻す
			double dirY = Sign(origMoveAmount.y);
			for (int iN = 1; iN <= 4; ++iN)
			{
				if (IsColliding(actor, blocks))
				{
					actor.setMoveAmount(origMoveAmount - Vec2{ 0, dirY });
					actor.applyMoveY();
				}
				else
				{
					break;
				}
			}

			// アクターの位置を確定
			actor.confirmPosition();
		}

		// プレイヤーと、アイテムや敵との当たり判定
		void CheckCollision(Player& player, Array<std::unique_ptr<Actor>>& actors, Array<std::unique_ptr<Block>>& blocks)
		{
			// プレイヤーとその他
			for (auto& actor : actors)
			{
				if (not IsCollidable(player, *actor)) continue;

				if (player.isCollidingWith(*actor))
				{
					player.onCollide(actor.get());
					actor->onCollide(&player);
				}
			}

			// その他どうし
			for (auto i = 0; i < actors.size(); ++i)
			{
				for (auto j = i + 1; j < actors.size(); ++j)
				{
					auto& ai = actors[i];
					auto& aj = actors[j];

					if (not IsCollidable(*ai, *aj)) continue;

					if (ai->isCollidingWith(*aj))
					{
						ai->onCollide(aj.get());
						aj->onCollide(ai.get());
					}
				}
			}

			// その他とブロック
			for (auto i = 0; i < actors.size(); ++i)
			{
				for (auto j = 0; j < blocks.size(); ++j)
				{
					auto& ai = actors[i];
					auto& aj = blocks[j];

					if (not IsCollidable(*ai, *aj)) continue;

					if (ai->isCollidingWith(*aj))
					{
						ai->onCollide(aj.get());
						aj->onCollide(ai.get());
					}
				}
			}
		}

	}

	MainScene::MainScene(const InitData& init)
		:
		CustomScene{ init },
		stageData_{},
		time_{ StartImmediately::Yes, Clock() },
		gauge_{ 0 },
		timerGaugeRecovery_{ 0.8s, StartImmediately::Yes, Clock() }
	{
		// ステージデータ読み込み
		LoadStage(U"1", stageData_);

		// プレイヤーを初期化
		getData().player = std::make_unique<Player>(stageData_.playerStartPos, getData());

		// アクターのリストを初期化
		getData().blocks.clear();
		getData().actors.clear();

		// アクターの初期配置
		SpawnActors(0, stageData_, getData());
	}

	MainScene::~MainScene()
	{
	}

	void MainScene::update()
	{
		// ポーズ
		if (getData().actionInput->down(Action::Pause))
		{
			if (GlobalClock::IsPaused())
			{
				GlobalClock::Start();
			}
			else
			{
				GlobalClock::Pause();
			}
		}

		auto& player = *getData().player;

		if (not GlobalClock::IsPaused())
		{
			// ポーズ状態ではないので、シーンを更新

			// プレイヤーを更新
			player.update();

			// アクター生成
			SpawnActors(time_.sF(), stageData_, getData());

			// ブロックを更新
			for (auto& block : getData().blocks)
			{
				block->update();
			}

			// その他アクターを更新
			//for (auto& actor : getData().actors)
			for (size_t i = 0; i < getData().actors.size(); ++i)
			{
				getData().actors[i]->update();
			}

			// プレイヤーの位置を調整
			UpdateActorPos(player, getData().blocks);

			// 敵の位置を調整
			for (auto& actor : getData().actors)
			{
				if (actor->type() == ActorType::EnemyChick)
				{
					UpdateActorPos(*actor, getData().blocks);
				}
			}

			// アクター同士の衝突判定
			CheckCollision(player, getData().actors, getData().blocks);

			// アクターの破棄
			getData().actors.remove_if([](const auto& a) { return not a->active(); });
			getData().blocks.remove_if([](const auto& a) { return not a->active(); });

			// 炎ゲージ自動回復
			if (timerGaugeRecovery_.reachedZero())
			{
				gauge_ = Saturate(gauge_ + 0.008);
				timerGaugeRecovery_.restart();
			}
		}

		// アクターの影のリストを作成
		shadowPosList_.clear();
		shadowPosList_.push_back(player.position().currentPos() + player.shadowOffset());
		for (size_t i = 0; i < getData().actors.size(); ++i)
		{
			const auto& actor = getData().actors[i];
			if (actor->tag() == ActorTag::Enemy || actor->tag() == ActorTag::Item)
			{
				shadowPosList_.push_back(actor->position().currentPos() + actor->shadowOffset());
			}
		}
	}

	void MainScene::draw() const
	{
		{
			const ScopedViewport2D viewport{ ActorsFieldViewportRect };

			// BG
			SceneRect.draw(Palette::Darkolivegreen.lerp(Palette::Cyan, 0.5));//

			// アクターの影
			for (const auto& shadowPos : shadowPosList_)
			{
				TextureAsset(U"Shadow").drawAt(shadowPos);
			}

			// ブロック
			for (const auto& block : getData().blocks)
			{
				block->draw();
			}

			// その他アクター
			for (const auto& actor : getData().actors)
			{
				actor->draw();
			}

			// プレイヤー
			getData().player->draw();
		}

		// HUD
		{
			// BG (0, 0), (0, 176)
			Rect{ 0, 0, Size{ SceneSize.x, TileSize } }.draw(ColorF{ 0.1 });
			Rect{ 0, 176, Size{ SceneSize.x, TileSize } }.draw(ColorF{ 0.1 });

			// ゲージ枠、ゲージ
			const Vec2 gaugePos{ 60, 174 };
			const double gaugeLength = 95 * gauge_;
			TextureAsset(U"Gauge")(0, 0, gaugeLength, 6).draw(gaugePos + Vec2{ 21, 7 }, ColorF{ 1 - 0.08 * Periodic::Sine0_1(0.3s, ClockTime()) });
			TextureAsset(U"GaugeFrame").draw(gaugePos);
		}

		// ポーズ中
		if (GlobalClock::IsPaused())
		{
			SceneRect.draw(ColorF{ 0, 0.5 });
			RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, 24 } }.draw(Palette::Darkred);
			FontAsset(U"px7812")(U"PAUSED - 休憩中").drawAt(SceneCenter);
		}
	}
}

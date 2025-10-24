#include "MainScene.h"
#include "Actor/Block.h"
#include "Actor/Effect.h"
#include "Actor/Enemy.h"
#include "Actor/Item.h"
#include "Actor/Player.h"
#include "Actor/MakeActor.h"
#include "Config/GameConfig.h"
#include "Core/DrawText.h"
#include "Core/DrawSprite.h"
#include "Core/Gauge.h"
#include "Core/Score.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"
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
			Actor* actor = nullptr;

			switch (spawn.type)
			{
			case ActorType::BlockSteel:
			case ActorType::BlockCanBreak:
			case ActorType::BlockGiftbox:
				gameData.blocks.push_back(std::make_unique<Block>(spawn.position, spawn.type, gameData, spawn.hasKey, spawn.secretRoute));
				actor = gameData.blocks.back().get();
				break;

			case ActorType::ItemChilipepper:
			case ActorType::ItemKey:
				gameData.actors.push_back(std::make_unique<Item>(spawn.position, spawn.type, gameData));
				actor = gameData.actors.back().get();
				break;

			case ActorType::EnemyChick:
				gameData.actors.push_back(std::make_unique<EnemyChick>(spawn.position, gameData, spawn.direction, spawn.subType, spawn.hasKey));
				actor = gameData.actors.back().get();
				break;

			default:
				break;
			}

			// 共通

			if (actor != nullptr)
			{
				if (spawn.life > 1e-3)
				{
					actor->setLife(spawn.life);
				}

				// 敵のスポーン時エフェクト
				if (actor->tag() == ActorTag::Enemy && spawn.time > 1e-3)
				{
					MakeSmokeFxN(gameData.actors, spawn.position, 3, {4,7}, {-8,0}, {0.8,1.3}, 0.3, 0.08);
				}
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
			for (auto i = 0; i < actors.size(); ++i)
			{
				auto& actor = actors[i];

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
		time_{ StartImmediately::No, Clock() },
		stageClearTime_{ 0 },
		timeStageStart_{ StartImmediately::Yes, Clock() },
		shadowPosList_{ Arg::reserve = 128 },
		timeGetKey_{ StartImmediately::No, Clock() },
		timeStageClear_{ StartImmediately::No, Clock() },
		timerGaugeMax_{ 0.50s, StartImmediately::No, Clock() },
		openedSecretRoute_{ false }
	{
		// ステージデータ読み込み
		LoadStage(getData().nextStageID, *getData().stageData);

		// 次のステージIDを設定
		getData().nextStageID = getData().stageData->nextStageID;

		// プレイヤーを初期化
		getData().player = std::make_unique<Player>(getData().stageData->playerStartPos, getData());

		// アクターのリストを初期化
		getData().blocks.clear();
		getData().actors.clear();

		// アクターの初期配置
		SpawnActors(0, *getData().stageData, getData());

		// ゲージ初期化
		getData().gauge = std::make_unique<Gauge>();

		// イベント購読
		GetDispatch().subscribe<GetKeyEvent, &MainScene::onGetKey_>(this);
		GetDispatch().subscribe<GaugeMaxEvent, &MainScene::onGaugeMax_>(this);
	}

	MainScene::~MainScene()
	{
		GetDispatch().unsubscribe<GetKeyEvent>(this);
		GetDispatch().unsubscribe<GaugeMaxEvent>(this);
	}

	void MainScene::update()
	{
		// ポーズの切替
		if (getData().actionInput->down(Action::Pause))
		{
			togglePause_();
		}

		if (timeStageStart_.isRunning())
		{
			// ◆ シーン開始～Ready表示
			updateStageStart_();
		}
		else if (timeGetKey_.isRunning())
		{
			// ◆ 鍵取得～ステージクリア表示～シーン遷移
			updateStageClear_();
		}
		else
		{
			// シーン開始表示中でなく、鍵取得状態でもない通常状態

			if (GlobalClock::IsPaused())
			{
				// ◆ ポーズ中
				updatePaused_();
			}
			else
			{
				// ◆ メインシーンの通常状態の更新
				updateMain_();
			}
		}

		// アクターの影のリストを作成
		makeCharacterShadows_();
	}

	void MainScene::togglePause_()
	{
		// ポーズ解除は優先的に行う
		if (GlobalClock::IsPaused())
		{
			GlobalClock::Start();
			return;
		}

		// ※ステージ開始時と、鍵取得～シーン遷移まではポーズしない
		if (timeStageStart_.isRunning()) return;
		if (timeGetKey_.isRunning()) return;

		GlobalClock::Pause();
	}

	void MainScene::updateStageStart_()
	{
		// ◆ シーン開始～Ready表示

		// ステージ開始時の "Ready" 表示
		if (timeStageStart_ > 2.5s)
		{
			timeStageStart_.reset();
			time_.start();
		}
	}

	void MainScene::updateStageClear_()
	{
		// ◆ 鍵取得～ステージクリア表示～シーン遷移

		// 鍵取得→少し待ってからクリア表示
		if (timeGetKey_ > 1.5s)
		{
			timeStageClear_.start();
		}

		// ステージクリア時の入力待ち→シーン遷移
		if (timeStageClear_ > 1.5s)
		{
			if (getData().actionInput->down(Action::Decide))
			{
				changeScene(U"MainScene", 0);
			}
		}

		// アクターのうちエフェクトは更新
		for (size_t i = 0; i < getData().actors.size(); ++i)
		{
			if (auto& actor = getData().actors[i];
				actor->tag() == ActorTag::Effect)
			{
				actor->update();
			}
		}
	}

	void MainScene::updatePaused_()
	{
		// ◆ ポーズ中
		//...
	}

	void MainScene::updateMain_()
	{
		// ◆ メインシーンの通常状態の更新

		// プレイヤーを更新
		auto& player = *getData().player;
		player.update();

		// アクター生成
		SpawnActors(time_.sF(), *getData().stageData, getData());

		// ブロックを更新
		auto& blocks = getData().blocks;
		for (auto& block : blocks)
		{
			block->update();
		}

		// その他アクターを更新
		auto& actors = getData().actors;
		for (size_t i = 0; i < actors.size(); ++i)
		{
			actors[i]->update();
		}

		// プレイヤーの位置を調整
		UpdateActorPos(player, blocks);

		// 敵の位置を調整
		for (auto& actor : actors)
		{
			// TODO: 位置調整の必要があるEnemyなどを列挙
			if (actor->type() == ActorType::EnemyChick ||
				actor->tag() == ActorTag::Item)
			{
				UpdateActorPos(*actor, blocks);
			}
		}

		// アクター同士の衝突判定
		CheckCollision(player, actors, blocks);

		// アクターの破棄
		actors.remove_if([](const auto& actor) { return not actor->active(); });
		blocks.remove_if([](const auto& block) { return not block->active(); });

		// 炎ゲージ更新
		getData().gauge->update();

		// ゲージマックス時ブロック破壊
		if (timerGaugeMax_.reachedZero())
		{
			timerGaugeMax_.reset();
			onTimerGaugeMax_();
		}
	}

	void MainScene::onTimerGaugeMax_()
	{
		// ブロックのうち SecretRoute プロパティが設定されているものを破壊
		for (size_t i = 0; i < getData().blocks.size(); ++i)
		{
			getData().blocks[i]->setInactiveIfSecret();
		}
	}

	void MainScene::makeCharacterShadows_()
	{
		shadowPosList_.clear();

		// プレイヤーの影
		shadowPosList_.push_back(getData().player->position().currentPos() + getData().player->shadowOffset());

		// 敵・アイテムの影
		for (size_t i = 0; i < getData().actors.size(); ++i)
		{
			const auto& actor = getData().actors[i];
			if (actor->tag() == ActorTag::Enemy ||
				actor->tag() == ActorTag::Item)
			{
				shadowPosList_.push_back(actor->position().currentPos() + actor->shadowOffset());
			}
		}
	}

	void MainScene::draw() const
	{
		// BG
		SceneRect.draw(Palette::Darkolivegreen.lerp(Palette::Cyan, 0.5));

		drawMain_();

		// HUD
		drawHUD_();

		// ステージ開始時
		if (timeStageStart_.isRunning())
		{
			drawStageStart_();
		}

		// ステージクリア時
		if (timeStageClear_.isRunning())
		{
			drawStageClear_();
		}

		// ポーズ中
		if (GlobalClock::IsPaused())
		{
			drawPaused_();
		}
	}

	void MainScene::drawMain_() const
	{
		const ScopedViewport2D viewport{ ActorsFieldViewportRect };

		// プレイヤー位置に追従するカメラ
		const auto camera = cameraTransform_();

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

	Transformer2D MainScene::cameraTransform_() const
	{
		const SizeF mapSize = getData().stageData->gridSize * TileSize;
		const Vec2 playerPos = getData().player->position().currentPos();
		RectF cameraRect{ Arg::center = playerPos, ActorsFieldViewportSize };

		if (openedSecretRoute_)
		{
			cameraRect.x = Clamp(cameraRect.x, 0.0, mapSize.x - ActorsFieldViewportSize.x);
			cameraRect.y = Clamp(cameraRect.y, 0.0, mapSize.y - ActorsFieldViewportSize.y);
		}
		else
		{
			cameraRect.x = Clamp(cameraRect.x, getData().stageData->cameraTopLeft.x, getData().stageData->cameraBottomRight.x - ActorsFieldViewportSize.x);
			cameraRect.y = Clamp(cameraRect.y, getData().stageData->cameraTopLeft.y, getData().stageData->cameraBottomRight.y - ActorsFieldViewportSize.y);
		}

		const Vec2 cameraMove = -cameraRect.pos;
		return Transformer2D{ Mat3x2::Translate(cameraMove) };
	}

	void MainScene::drawHUD_() const
	{
		// BG
		const ColorF bgColor{ 0.1 };
		const auto regionHudU = Rect{ Arg::topLeft = SceneRect.tl(), Size{ SceneSize.x, TileSize } }.draw(bgColor);
		const auto regionHudD = Rect{ Arg::bottomRight = SceneRect.br(), Size{ SceneSize.x, TileSize } }.draw(bgColor);

		// スコア
		const ColorF scoreColor = (getData().score->animating()) ? ColorF{ Palette::White.lerp(Palette::Red, 0.8 * Periodic::Square0_1(0.06s, ClockTime())) } : ColorF{ 0.9 - 0.1 * Periodic::Square0_1(0.25s, ClockTime()) };
		DrawText(U"px7812m", U"{:08d}"_fmt(getData().score->displayScore()), Arg::center = regionHudU.center(), ColorF{ 0.3 });
		DrawText(U"px7812m", U"{:-8d}"_fmt(getData().score->displayScore()), Arg::center = regionHudU.center(), scoreColor, ColorF{ Palette::Darkred, 0.6 - 0.1 * Periodic::Square0_1(0.25s, ClockTime()) });

		// 経過時間
		//FontAsset(U"px7812m")(U"{:02d}:{:02d}"_fmt(time_.min(), time_.s() % 60)).draw(Arg::rightCenter = regionHudU.rightCenter() + Vec2{ -1, 0 }, Palette::Gray);

		// ゲージ枠、ゲージ
		getData().gauge->draw();
	}

	void MainScene::drawStageStart_() const
	{
		const double height = 24 * EaseInQuad(Saturate(timeStageStart_.sF() / 0.3));
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, height } }.draw(ColorF{ Palette::Darkred, 0.9 });

		const String text = (timeStageStart_ < 1.5s) ? getData().stageData->name : U"Ready";
		const Vec2 textPos = SceneCenter + Vec2{ 400 * (1 - EaseOutExpo(Saturate(timeStageStart_.sF() / 1.0))), 0 };

		DrawText(U"px7812", text, Arg::center = textPos, ColorF{ 1.0 - 0.3 * Periodic::Square0_1(0.25s, ClockTime()) });
	}

	void MainScene::drawStageClear_() const
	{
		// 帯
		const double height = 24 * EaseInQuad(Saturate(timeStageClear_.sF() / 0.3));
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, height } }.draw(ColorF{ Palette::Darkred, 0.9 });

		// テキスト
		const String text = U"{}  Clear!  ({:02d}:{:02d})"_fmt(getData().stageData->name, (int)(stageClearTime_) / 60, (int)stageClearTime_ % 60);
		const Vec2 textPos = SceneCenter + Vec2{ 400 * (1 - EaseOutExpo(Saturate(timeStageClear_.sF() / 1.0))), 0 };
		DrawText(U"px7812", text, Arg::center = textPos, Palette::Yellow.lerp(Palette::Black, 0.2 * Periodic::Square0_1(0.25s, ClockTime())));

		// 入力待ち表示
		if (timeStageClear_ > 1.5s)
		{
			DrawSprite(*getData().appSetting, U"WhiteArrowDown", 0.5s, false, SceneCenter + Vec2{ SceneSize.x / 2 - 16 , 4 + 1 * Periodic::Square0_1(0.5s, ClockTime()) });
		}
	}

	void MainScene::drawPaused_() const
	{
		// 背景を暗くする
		SceneRect.draw(ColorF{ 0, 0.5 });

		// 帯
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, 24 } }.draw(Palette::Darkred);

		DrawText(U"px7812", U"PAUSED - 休憩中", Arg::center = SceneCenter, Palette::White);
	}

	void MainScene::onGetKey_()
	{
		// プレイヤーが鍵をとった
		// ゲームクリア表示へ...
		timeGetKey_.start();

		// TODO: クリアタイム
		//...
		stageClearTime_ = time_.sF();
	}

	void MainScene::onGaugeMax_()
	{
		if (not openedSecretRoute_)
		{
			openedSecretRoute_ = true;

			// ゲージマックス時ゲージエフェクト
			getData().gauge->startDrawMaxEffect();

			// 時間差でブロック破壊
			timerGaugeMax_.start();
		}
	}
}

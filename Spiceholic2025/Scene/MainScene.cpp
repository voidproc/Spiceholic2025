#include "MainScene.h"
#include "Actor/Block.h"
#include "Actor/Effect.h"
#include "Actor/Enemy.h"
#include "Actor/Item.h"
#include "Actor/Player.h"
#include "Actor/MakeActor.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/Camera.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Core/DrawSprite.h"
#include "Core/Gauge.h"
#include "Core/Score.h"
#include "Core/StageRecord.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"
#include "Input/ActionInput.h"
#include "Stage/Stage.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		// メニュー

		enum class MenuItemType
		{
			Back,
			Retry,
			Title,
			Next,
			Skip,
		};

		struct MenuItem
		{
			MenuItemType type;
			StringView text;
			StringView desc;
		};

		constexpr std::array<MenuItem, 4> PauseMenuItemList = { {
			{ MenuItemType::Back, U"Back to Game"_sv, U"ゲームに戻ります"_sv },
			{ MenuItemType::Retry, U"Retry This Stage"_sv, U"このステージをやり直します"_sv },
			{ MenuItemType::Title, U"Quit to Title"_sv, U"タイトル画面に戻ります"_sv },
			{ MenuItemType::Next, U"Skip This Stage"_sv, U"このステージをとばして次へ（ゲームジャム仕様）"_sv },
		} };

		constexpr std::array<MenuItem, 2> ClearMenuItemList = { {
			{ MenuItemType::Next, U"To Next Stage"_sv, U"次のステージに進みます"_sv },
			{ MenuItemType::Retry, U"Retry This Stage"_sv, U"このステージをやり直します"_sv },
		} };


		// 特殊アイテム関連
		struct HUDSpItemInfo
		{
			Vec2 offset;
			String textureName;
			double num;
		};

		const HashTable<ActorType, HUDSpItemInfo> HUDSpItemInfoMap = {
			{ ActorType::ItemHabanero, HUDSpItemInfo{.offset = Vec2{20 * 0.35, 0}, .textureName = U"ItemHabanero", .num = 1 } },
			{ ActorType::ItemCurry, HUDSpItemInfo{.offset = Vec2{20 * 1.15, 0}, .textureName = U"ItemCurry", .num = 2 } },
			{ ActorType::ItemMapo, HUDSpItemInfo{.offset = Vec2{20 * 2.1, 0}, .textureName = U"ItemMapo", .num = 3 } },
		};


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
			case ActorType::BlockCanBreakGray:
			case ActorType::BlockSpike:
			case ActorType::BlockGiftbox:
				gameData.blocks.push_back(std::make_unique<Block>(spawn.position, spawn.type, gameData, spawn.bringItems, spawn.secretRoute, spawn.endingRoute));
				actor = gameData.blocks.back().get();
				break;

			case ActorType::ItemChilipepper:
			case ActorType::ItemKey:
			case ActorType::ItemKeyEnd:
			case ActorType::ItemCurry:
			case ActorType::ItemHabanero:
			case ActorType::ItemMapo:
			case ActorType::ItemWasabi:
				gameData.actors.push_back(std::make_unique<Item>(spawn.position, spawn.type, gameData));
				actor = gameData.actors.back().get();
				break;

			case ActorType::EnemyChick:
			case ActorType::EnemyChickFly:
				gameData.actors.push_back(std::make_unique<EnemyChick>(spawn.position, gameData, spawn.direction, spawn.subType, spawn.bringItems, spawn.type == ActorType::EnemyChickFly));
				actor = gameData.actors.back().get();
				break;

			case ActorType::FxNotice:
				gameData.actors.push_back(std::make_unique<FxNotice>(gameData, spawn.position));
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
				if ((actor->tag() == ActorTag::Enemy || actor->tag() == ActorTag::Block) && spawn.time > 1e-3)
				{
					MakeSmokes(gameData, spawn.position);

					// SE
					PlayAudioOneShot(U"Spawn");
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
				if (actor.tag() == ActorTag::Weapon && block->type() == ActorType::BlockSpike) continue;
				if (actor.type() == ActorType::EnemyChickFly && block->type() == ActorType::BlockSpike) continue;

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

		template <class T>
		void RemoveInactiveActors(Array<std::unique_ptr<T>>& actors)
		{
			actors.remove_if([](const auto& actor) { return not actor->active(); });
		}

		int EnemyCount(const Array<std::unique_ptr<Actor>>& actors)
		{
			int enemyCount = 0;

			for (const auto& actor : actors)
			{
				if (actor->tag() == ActorTag::Enemy)
				{
					++enemyCount;
				}
			}

			return enemyCount;
		}

		int KeyCount(const Array<std::unique_ptr<Actor>>& actors)
		{
			int keyCount = 0;

			for (const auto& actor : actors)
			{
				if (actor->type() == ActorType::ItemKey)
				{
					++keyCount;
				}
			}

			return keyCount;
		}

		auto CurrentBGM(const StageData& stageData)
		{
			return AudioAsset(stageData.stageGroup.bgm);
		}
	}

	MainScene::MainScene(const InitData& init)
		:
		CustomScene{ init },
		smoothCameraRect_{},
		time_{ StartImmediately::No, Clock() },
		stageClearTime_{ 0 },
		timeStageStart_{ StartImmediately::Yes, Clock() },
		shadowPosList_{ Arg::reserve = 128 },
		timeDestroyAllEnemy_{ StartImmediately::No, Clock() },
		initialEnemyCount_{},
		initialKeyCount_{},
		timeGetKey_{ StartImmediately::No, Clock() },
		timeStageClear_{ StartImmediately::No, Clock() },
		clearMenu_{ ClearMenuItemList.size() },
		timeGetLastKey_{ StartImmediately::No, Clock() },
		openedEndingRoute_{ false },
		timerGaugeMax_{ 0.50s, StartImmediately::No, Clock() },
		timePause_{ StartImmediately::No },
		pauseMenu_{ PauseMenuItemList.size() },
		cameraShakeIntensity_{ 1.0 },
		timerCameraShake_{ 0.08s, StartImmediately::No, Clock() },
		snow_{ Arg::reserve = 256 },
		noise_{},
		timerSnow_{ 0.5s, StartImmediately::Yes, Clock() },
		timerGetSpItem_{ 0.4s, StartImmediately::No, Clock() },
		gotSpItem_{ ActorType::None }
	{
		// ステージデータ読み込み
		LoadStage(getData().nextStageID, *getData().stageData);
		// ステージグループ
		getData().stageData->stageGroup = getData().appSetting->get().stageGroupInfo[getData().stageData->stageID];
		// Notice
		{
			Array<std::pair<double, Vec2>> noticeList;
			for (const auto& spawn : getData().stageData->actorSpawns)
			{
				if (spawn.time < 1.2) continue;

				noticeList.push_back(std::make_pair(spawn.time - 1.2, spawn.position));
			}
			for (const auto& notice : noticeList)
			{
				ActorSpawnInfo info;
				info.time = notice.first;
				info.position = notice.second;
				info.type = ActorType::FxNotice;
				getData().stageData->actorSpawns.push_back(info);
			}
		}

		// 次のステージIDを設定
		getData().nextStageID = getData().stageData->nextStageID;

		// プレイヤーを初期化
		getData().player = std::make_unique<Player>(getData().stageData->playerStartPos, getData());

		// アクターのリストを初期化
		getData().blocks.clear();
		getData().actors.clear();

		// アクターの初期配置
		SpawnActors(0, *getData().stageData, getData());
		initialEnemyCount_ = EnemyCount(getData().actors);
		initialKeyCount_ = KeyCount(getData().actors);

		// ゲージ初期化
		getData().gauge = std::make_unique<Gauge>();

		// カメラ位置
		smoothCameraRect_ = CameraRect(getData());

		// ポーズメニュー
		pauseMenu_ = Menu{ PauseMenuItemList.size() - ((getData().stageData->stageID == U"12") ? 1 : 0)};

		// イベント購読
		GetDispatch().subscribe<GetKeyEvent, &MainScene::onGetKey_>(this);
		GetDispatch().subscribe<GaugeMaxEvent, &MainScene::onGaugeMax_>(this);
		GetDispatch().subscribe<CameraShakeEvent, &MainScene::onCameraShake_>(this);
		GetDispatch().subscribe<GetSpecialItemEvent, &MainScene::onGetSpecialItem_>(this);
		GetDispatch().subscribe<GetLastKeyEvent, &MainScene::onGetLastKey_>(this);
	}

	MainScene::~MainScene()
	{
		GetDispatch().unsubscribe<GetKeyEvent>(this);
		GetDispatch().unsubscribe<GaugeMaxEvent>(this);
		GetDispatch().unsubscribe<CameraShakeEvent>(this);
		GetDispatch().unsubscribe<GetSpecialItemEvent>(this);
		GetDispatch().unsubscribe<GetLastKeyEvent>(this);
	}

	void MainScene::update()
	{
		if (timeStageStart_.isRunning())
		{
			// ◆ シーン開始～Ready表示
			updateStageStart_();
		}
		else if (timeGetKey_.isRunning() || timeDestroyAllEnemy_.isRunning())
		{
			// ◆ 敵全滅～ステージクリア表示～シーン遷移
			//    または
			//    鍵取得～ステージクリア表示～シーン遷移
			updateStageClear_();
		}
		else if (timeGetLastKey_.isRunning())
		{
			// ◆ 最終ステージの鍵を取得
			updateGameClear_();
		}
		else
		{
			// シーン開始表示中でなく、ステージクリア遷移状態でもない通常状態

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

			// BGM
			if (CurrentBGM(*getData().stageData).isPaused())
			{
				CurrentBGM(*getData().stageData).play(MixBus1, 0.05s);
			}

			return;
		}

		// ※ステージ開始時と、鍵取得～シーン遷移まではポーズしない
		if (timeStageStart_.isRunning()) return;
		if (timeGetKey_.isRunning()) return;

		GlobalClock::Pause();

		pauseMenu_.setSelectedIndex(0);
		timePause_.restart();

		//SE
		PlayAudioOneShot(U"Pause1");

		// BGM
		CurrentBGM(*getData().stageData).pause(0.05s);
	}

	void MainScene::updateStageStart_()
	{
		// ◆ シーン開始～Ready表示

		// ステージ開始時の "Ready" 表示
		if (timeStageStart_ > 2.5s)
		{
			timeStageStart_.reset();
			time_.start();

			// BGM
			if (getData().stageData->stageGroup.bgm != U"")
			{
				CurrentBGM(*getData().stageData).play(MixBus1, 0.7s);
			}
		}
	}

	void MainScene::updateStageClear_()
	{
		// ◆ 鍵取得～ステージクリア表示～シーン遷移

		// 鍵取得 or 敵全滅 → 少し待ってからクリア表示
		if (const auto time = Max(timeGetKey_.elapsed(), timeDestroyAllEnemy_.elapsed());
			time > 1.5s)
		{
			timeStageClear_.start();

			// BGM
			StopAllBgm(0.7s);
		}

		// ステージクリア時の入力待ち→メニュー表示
		if (timeStageClear_ > 1.5s && not clearMenu_.decideTimer().isRunning())
		{
			if (getData().actionInput->down(Action::MoveUp))
			{
				clearMenu_.selectPrevious();

				// SE
				PlayAudioOneShot(U"Select2");
			}
			else if (getData().actionInput->down(Action::MoveDown))
			{
				clearMenu_.selectNext();

				// SE
				PlayAudioOneShot(U"Select2");
			}

			if (getData().actionInput->down(Action::Decide))
			{
				clearMenu_.decide();

				// SE
				PlayAudioOneShot(U"Decide1");
			}
		}

		if (clearMenu_.decideTimer().reachedZero())
		{
			clearMenu_.resetDecideTimer();

			if (const auto& selected = ClearMenuItemList[clearMenu_.selectedIndex()];
				selected.type == MenuItemType::Next)
			{
				// シーン遷移する前に今のスコアを保存しておく（元に戻す用）
				getData().stageStartScore = getData().score->currentScore();

				changeScene(U"MainScene", 0);
			}
			else if (selected.type == MenuItemType::Retry)
			{
				// このステージをやり直す
				getData().score->set(getData().stageStartScore, true);
				getData().nextStageID = getData().stageData->stageID;
				changeScene(U"MainScene", 0);
			}
		}

		// アクターのうちアイテム、エフェクトは更新
		for (size_t i = 0; i < getData().actors.size(); ++i)
		{
			if (auto& actor = getData().actors[i];
				actor->tag() == ActorTag::Effect ||
				actor->tag() == ActorTag::Item)
			{
				actor->update();
			}
		}

		RemoveInactiveActors(getData().actors);
	}

	void MainScene::updateGameClear_()
	{
		// ◆ 最終ステージの鍵を取得

		// カメラ位置
		const auto camRect = CameraRect(getData());
		smoothCameraRect_.pos += (camRect.pos - smoothCameraRect_.pos).limitLength(4.0 * 60 * Scene::DeltaTime());

		if (timeGetLastKey_ > 11s + 28s + 3s)
		{
			if (getData().actionInput->down(Action::Decide))
			{
				// 次のステージを初期化
				getData().nextStageID = U"1";

				// 暫定
				changeScene(U"TitleScene", 0s);
				return;
			}
		}
		else if (timeGetLastKey_ > 11s + 28s)
		{
			AudioAsset(U"Area1").stop(3s);
		}
		else if (timeGetLastKey_ > 11s)
		{
			if (not AudioAsset(U"Area1").isPlaying())
			{
				AudioAsset(U"Area1").play(3s, MixBus1);
			}
		}
		else if (timeGetLastKey_ > 1.5s)
		{
			if (not openedEndingRoute_)
			{
				// ブロックのうち EndingRoute プロパティが設定されているものを破壊
				for (size_t i = 0; i < getData().blocks.size(); ++i)
				{
					getData().blocks[i]->setInactiveIfEndingRoute();
				}

				openedEndingRoute_ = true;
			}
		}

		// プレイヤーを更新
		auto& player = *getData().player;
		player.update();

		// ブロックを更新
		auto& blocks = getData().blocks;
		for (auto& block : blocks)
		{
			block->update();
		}

		// アクターのうちアイテム、エフェクトは更新
		for (size_t i = 0; i < getData().actors.size(); ++i)
		{
			if (auto& actor = getData().actors[i];
				actor->tag() == ActorTag::Effect ||
				actor->tag() == ActorTag::Item)
			{
				actor->update();
			}
		}

		// プレイヤーの位置を調整
		UpdateActorPos(player, blocks);

		RemoveInactiveActors(getData().actors);
		RemoveInactiveActors(getData().blocks);

		// 辛ゲージ更新
		getData().gauge->update();
	}

	void MainScene::updatePaused_()
	{
		// ◆ ポーズ中

		// メニュー選択、項目決定、ポーズ解除
		// 項目決定されていたら実行しない
		if (not pauseMenu_.decideTimer().isRunning())
		{
			// メニュー選択
			if (getData().actionInput->down(Action::MoveUp))
			{
				pauseMenu_.selectPrevious();

				// SE
				PlayAudioOneShot(U"Select2");
			}
			else if (getData().actionInput->down(Action::MoveDown))
			{
				pauseMenu_.selectNext();

				// SE
				PlayAudioOneShot(U"Select2");
			}

			// 項目決定
			if (getData().actionInput->down(Action::Decide))
			{
				pauseMenu_.decide();

				// SE
				PlayAudioOneShot(U"Decide1");
			}

			// ポーズ解除
			if (getData().actionInput->down(Action::Pause))
			{
				// ゲームに戻る を選択
				pauseMenu_.setSelectedIndex(0);
				pauseMenu_.decide();

				// SE
				PlayAudioOneShot(U"Decide1");
			}
		}

		if (pauseMenu_.decideTimer().reachedZero())
		{
			pauseMenu_.resetDecideTimer();

			if (const auto& selected = PauseMenuItemList[pauseMenu_.selectedIndex()];
				selected.type == MenuItemType::Back)
			{
				// ゲームに戻る
			}
			else if (selected.type == MenuItemType::Retry)
			{
				// このステージをやり直す
				getData().score->set(getData().stageStartScore, true);
				getData().nextStageID = getData().stageData->stageID;

				// BGM
				StopAllBgm();

				changeScene(U"MainScene", 0);
			}
			else if (selected.type == MenuItemType::Title)
			{
				// タイトルに戻る

				// 次のステージIDを初期化
				getData().nextStageID = U"1";

				// BGM
				StopAllBgm();

				changeScene(U"TitleScene", 0);
			}
			else if (selected.type == MenuItemType::Next)
			{
				// このステージをスキップ（次のステージへ）

				// 次のステージIDを初期化
				getData().nextStageID = getData().stageData->nextStageID;

				// BGM
				StopAllBgm();

				changeScene(U"MainScene", 0);
			}

			// ポーズ解除
			togglePause_();
		}
	}

	void MainScene::updateMain_()
	{
		// ◆ メインシーンの通常状態の更新

		// カメラ位置
		const auto camRect = CameraRect(getData());
		smoothCameraRect_.pos += (camRect.pos - smoothCameraRect_.pos).limitLength(4.0 * 60 * Scene::DeltaTime());

		// ポーズの切替
		if (getData().actionInput->down(Action::Pause))
		{
			togglePause_();
		}

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
			if ((actor->type() == ActorType::EnemyChick) ||
				(actor->type() == ActorType::EnemyChickFly))
			{
				UpdateActorPos(*actor, blocks);
			}
		}

		// アクター同士の衝突判定
		CheckCollision(player, actors, blocks);

		// アクターの破棄
		RemoveInactiveActors(actors);
		RemoveInactiveActors(blocks);

		// 敵全滅判定
		if (EnemyCount(getData().actors) == 0 && initialEnemyCount_ != 0 && initialKeyCount_ == 0)
		{
			onDestroyAllEnemies_();
		}

		// 辛ゲージ更新
		getData().gauge->update();

		// ゲージマックス時ブロック破壊とスクロールロック解除
		if (timerGaugeMax_.reachedZero())
		{
			timerGaugeMax_.reset();
			onTimerGaugeMax_();
		}

		// 雪
		if (getData().stageData->stageGroup.group == StageGroupType::Snow)
		{
			updateSnow_();
		}
	}

	void MainScene::onTimerGaugeMax_()
	{
		// ブロックのうち SecretRoute プロパティが設定されているものを破壊
		for (size_t i = 0; i < getData().blocks.size(); ++i)
		{
			getData().blocks[i]->setInactiveIfSecretRoute();
		}

		// スクロールロック解除
		getData().stageData->openedSecretRoute = true;
	}

	void MainScene::updateSnow_()
	{
		const auto camRect = CameraRect(getData());

		if (timerSnow_.reachedZero())
		{
			timerSnow_.restart();

			for (int i = 0; i < 8; ++i)
			{
				snow_.push_back(Snow{
					Vec2{ i * (SceneSize.x + 140) / 8 - 70 + Random(-32, 32), Random(-16, 0) } + camRect.pos,
					time_.sF() + i + Random() * 8,
					true
					});
			}
		}

		for (auto& s : snow_)
		{
			s.pos.x += noise_.noise1D(time_.sF() * 2.0 + s.noisePos) * 0.5;
			s.pos.y += 1.0 * 60 * (0.6 + 0.1 * Periodic::Sine0_1(1.5s, ClockTime())) * Scene::DeltaTime();
			s.active = RandomBool(0.995);
		}

		const auto camRect2 = camRect.stretched(32, 80, 0, 80);
		snow_.remove_if([&](const auto& s) { return not camRect2.intersects(s.pos) || not s.active; });
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
		SceneRect.draw(ColorF{ 0 });

		drawMain_();

		// HUD
		drawHUD_();

		// ステージ開始時
		if (timeStageStart_.isRunning())
		{
			// シーン開始時フェードイン
			if (timeStageStart_ < 0.3s)
			{
				const double t = Saturate(timeStageStart_.sF() / 0.3);
				SceneRect.draw(ColorF{ Palette::Whitesmoke, 1 - EaseOutSine(t) });
			}

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

		// ゲームクリア時
		if (timeGetLastKey_.isRunning())
		{
			if (timeGetLastKey_ > 9s)
			{
				SceneRect.draw(ColorF{ 0, 0.85 * EaseInOutSine(Saturate((timeGetLastKey_.sF() - 9.0)) / 1.8) });
			}

			Array<Array<String>> textList = {
				{
					U"Siv3D ゲームジャム 2025 投稿作品",
					U"Blazing Spiceholic",
				},
				{
					U"制作",
					U"",
					U"voidProc",
				},
				{
					U"出演",
					U"",
					U"ドラゴ子",
					U"ダークひよこ",
					U"フライングダークひよこ",
				},
				{
					U"ドラゴ子がみつけた辛いもの",
					U"",
					U"とうがらし",
				},
				{
					U"Thank you for playing!",
				}
			};

			textList[3].push_back(getData().specialItems.contains(ActorType::ItemHabanero) ? U"ハバネロ" : U"???");
			textList[3].push_back(getData().specialItems.contains(ActorType::ItemCurry) ? U"カレーライス" : U"???");
			textList[3].push_back(getData().specialItems.contains(ActorType::ItemMapo) ? U"麻婆豆腐" : U"???");
			textList[3].push_back(U"わさび");

			if (const double t = timeGetLastKey_.sF() - 12.0;
				t > 0)
			{
				const auto page = Min(static_cast<size_t>(t / 7), textList.size() - 1);
				constexpr double LineHeight = 18;

				ColorF color = LightYellow;

				for (const auto [index, text] : Indexed(textList[page]))
				{
					if (page > 0 && index > 0) color = Palette::Whitesmoke;

					DrawText(U"px7812", text, Arg::center = SceneCenter + Vec2{ 0, LineHeight * index - LineHeight * (textList[page].size() - 1) / 2.0 }, color);
				}
			}

			// 入力待ち
			if (timeGetLastKey_ > 11s + 28s + 3s)
			{
				DrawSprite(*getData().appSetting, U"WhiteArrowDown", 0.5s, false, SceneRect.br() - Vec2{ 16, 16 + Periodic::Square0_1(0.5s) });
			}
		}
	}

	void MainScene::drawMain_() const
	{
		const ScopedViewport2D viewport{ ActorsFieldViewportRect };

		// プレイヤー位置に追従するカメラ
		const auto camera = cameraScrollTransform_();

		// カメラ揺れ
		const Vec2 shakeAmount = timerCameraShake_.isRunning() ? (2.0 * Vec2{ 1, 0 } * cameraShakeIntensity_ * Periodic::Sine1_1(0.04s, ClockTime()) * timerCameraShake_.progress1_0()) : Vec2{};
		const Transformer2D shake{ Mat3x2::Translate(shakeAmount) };

		// 地面
		{
			const ScopedRenderStates2D sampler{ SamplerState::RepeatNearest };
			StringView textureName = getData().stageData->stageGroup.groundTexture;
			TextureAsset(textureName).mapped(getData().stageData->gridSize * TileSize).draw();
		}

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

		// 雪
		for (const Snow& s : snow_)
		{
			const Vec2 pos = s.pos - CameraRect(getData()).pos;
			const double alpha = (EaseInQuad(Saturate(time_.sF() / 4.0))) * (1 - Saturate((pos.y - (ActorsFieldViewportSize.y - 48)) / 48.0));
			ScopedColorMul2D mul{ 1, alpha * (0.5 + 0.3 * Periodic::Sine0_1(0.09s, ClockTime() + s.noisePos)) };
			DrawSprite(*getData().appSetting, U"Snow", 0.7s, false, s.pos, s.noisePos + time_.sF());
		}
	}

	Transformer2D MainScene::cameraScrollTransform_() const
	{
		const Vec2 cameraMove = -smoothCameraRect_.pos;
		return Transformer2D{ Mat3x2::Translate(cameraMove) };
	}

	void MainScene::drawHUD_() const
	{
		// BG
		const ColorF bgColor{ 0.1 };
		const auto regionHudU = Rect{ Arg::topLeft = SceneRect.tl(), Size{ SceneSize.x, TileSize } }.draw(bgColor);
		const auto regionHudD = Rect{ Arg::bottomRight = SceneRect.br(), Size{ SceneSize.x, TileSize } }.draw(bgColor);

		TextureAsset(U"Head")(0, (int)getData().stageData->stageGroup.group * 16, 50, 16).draw(1, 0);

		// ステージ名
		constexpr std::array<ColorF, 5> textColorList = {
			Palette::Forestgreen,
			Palette::Olive,
			Palette::Dodgerblue,
			Palette::Red,
			Palette::Red,
		};
		constexpr std::array<ColorF, 5> shadowColorList = {
			Palette::Green,
			Palette::Darkolivegreen,
			Palette::Steelblue.lerp(Palette::Black, 0.3),
			Palette::Crimson,
			Palette::Crimson,
		};

		const auto group = FromEnum(getData().stageData->stageGroup.group);
		const ColorF textColor{ Palette::Whitesmoke.lerp(textColorList[group], 0.1 * Periodic::Square0_1(0.25s, ClockTime()))};
		const ColorF shadowColor{ shadowColorList[group].lerp(Palette::Silver, 0.3), 0.8 - 0.1 * Periodic::Square0_1(0.25s, ClockTime())};
		const String& text = U"ステージ {} {}"_fmt(getData().stageData->stageID, getData().stageData->stageGroup.subtitle);
		DrawText(U"px7812", text, Arg::center = regionHudU.center(), textColor, shadowColor);

		// 経過時間
		const int32 timeSec = (timeDestroyAllEnemy_.isRunning() || timeGetKey_.isRunning() || timeGetLastKey_.isRunning() || timeStageClear_.isRunning()) ? static_cast<int32>(stageClearTime_) : time_.s();
		const String textTime = U"{:02d}:{:02d}"_fmt(timeSec / 60, timeSec % 60);
		DrawText(U"m3x6", textTime, Arg::rightCenter = regionHudU.rightCenter() - Vec2{ 4, 2 }, textColor, shadowColor);


		// ゲージ枠、ゲージ
		getData().gauge->draw();

		// 特殊アイテム
		{
			for (const auto& type : getData().specialItems)
			{
				const auto& info = HUDSpItemInfoMap.at(type);

				ScopedColorMul2D mul{ 1, 0.75 + 0.1 * Periodic::Jump0_1(0.1s, ClockTime()) };
				ScopedColorAdd2D add{ 0.1 * Periodic::Jump0_1(1 + info.num * 0.2, ClockTime()) + (type == gotSpItem_ && timerGetSpItem_.isRunning()) * 0.9 * Periodic::Square0_1(0.09s, ClockTime())};

				DrawSprite(*getData().appSetting, info.textureName, 1s, false, regionHudD.leftCenter() + info.offset);
			}

			if (timerGetSpItem_.isRunning())
			{
				const auto& info = HUDSpItemInfoMap.at(gotSpItem_);
				Circle{ regionHudD.leftCenter() + info.offset, 8 + 20 * EaseOutQuad(timerGetSpItem_.progress0_1()) }.drawFrame(2.0 - 1.5 * timerGetSpItem_.progress0_1(), ColorF{ 1, 0.5 + 0.5 * Periodic::Square0_1(0.8s, ClockTime())});
			}
		}
	}

	void MainScene::drawStageStart_() const
	{
		const double height = 24 * EaseInQuad(Saturate(timeStageStart_.sF() / 0.3));
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, height } }.draw(ColorF{ Palette::Darkred, 0.9 });

		const String text = (timeStageStart_ < 1.5s) ? getData().stageData->name : U"Ready";
		const Vec2 textPos = SceneCenter + Vec2{ 400 * (1 - EaseOutExpo(Saturate(timeStageStart_.sF() / 1.0))), 0 };

		DrawText(U"px7812", text, Arg::center = textPos, Palette::Whitesmoke.lerp(Palette::Indianred, 0.4 * Periodic::Square0_1(0.25s, ClockTime())));
	}

	void MainScene::drawStageClear_() const
	{
		{
			// テキストと帯のY移動
			const double t = Saturate((timeStageClear_.sF() - 1.0) / 0.5);

			// 帯
			const double height = 24 * EaseInOutQuad(Saturate(timeStageClear_.sF() / 0.3)) + 56 * EaseInOutQuad(t);
			RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, height } }.draw(ColorF{ Palette::Darkred, 0.9 });

			// テキスト
			const String text = U"{}  Clear!  ({:02d}:{:02d})"_fmt(getData().stageData->name, (int)(stageClearTime_) / 60, (int)stageClearTime_ % 60);
			const Vec2 textPos = SceneCenter + Vec2{ 400 * (1 - EaseOutExpo(Saturate(timeStageClear_.sF() / 1.0))), 0 };
			const Vec2 textPosUp{ 0, -26 * EaseInOutQuad(t) };
			DrawText(U"px7812", text, Arg::center = textPos + textPosUp, LightYellow.lerp(Palette::Black, 0.2 * Periodic::Square0_1(0.25s, ClockTime())));
		}

		// メニュー表示
		if (timeStageClear_ > 1.5s)
		{
			constexpr int LineHeight = 18;
			constexpr Color TextColor = Palette::Lightcoral.lerp(Palette::White, 0.3);
			constexpr Color SelectedColor = Palette::Whitesmoke;

			for (const auto [index, item] : Indexed(ClearMenuItemList))
			{
				const Vec2 itemCenter = SceneCenter + Vec2{ 0, 10 } + Vec2{ 0, ((ClearMenuItemList.size() - 1) / -2.0 + index) * LineHeight };
				const bool selected = (index == clearMenu_.selectedIndex());

				// 選択行
				if (selected)
				{
					const Vec2 vibrate = MenuMoveCursorVibrateVertical(clearMenu_);
					const double alpha = (0.2 + 0.2 * Periodic::Jump0_1(0.75s)) * (0.8 + 0.2 * Periodic::Square0_1(32ms));
					RectF{ Arg::center = itemCenter + vibrate, SizeF{ SceneSize.x, 16 } }.draw(ColorF{ 1, alpha });
				}

				const ColorF textColor = (selected && clearMenu_.decideTimer().isRunning()) ?
					(LightYellow.lerp(Palette::Darkred, 0.3 * Periodic::Square0_1(0.12s))) :
					(selected ? SelectedColor : TextColor);
				DrawText(U"px7812", item.text, Arg::center = itemCenter, textColor);
			}

			const Vec2 descCenter = SceneRect.center().withY(SceneSize.y - LineHeight * 1);
			RectF{ Arg::center = descCenter, SizeF{ SceneSize.x, 16 } }.draw(Palette::Black);
			DrawText(U"px7812", ClearMenuItemList[clearMenu_.selectedIndex()].desc, Arg::center = descCenter, Palette::Silver);
		}
	}

	void MainScene::drawPaused_() const
	{
		// 背景を暗くする
		SceneRect.draw(ColorF{ 0, 0.9 });

		constexpr int LineHeight = 20;

		{
			const double t = Saturate(timePause_.sF() / 0.25);
			const double ease = 0.6 * EaseOutBack(t) + 0.4 * EaseOutQuad(t);
			Transformer2D trans{ Mat3x2::Translate(0, 500 * (1 - ease)) };

			for (const auto [index, item] : Indexed(PauseMenuItemList))
			{
				if ((getData().stageData->stageID == U"12") && (index == 3)) continue;

				const Vec2 itemCenter = SceneCenter + Vec2{ 0, ((PauseMenuItemList.size() - 1) / -2.0 + index) * LineHeight + ((index > 2) ? 8 : 0) };
				const bool selected = (index == pauseMenu_.selectedIndex());

				// 選択行
				if (selected)
				{
					const Vec2 vibrate = MenuMoveCursorVibrateVertical(pauseMenu_);
					const double alpha = (0.2 + 0.2 * Periodic::Jump0_1(0.75s)) * (0.8 + 0.2 * Periodic::Square0_1(32ms));
					RectF{ Arg::rightCenter = itemCenter + vibrate, SizeF{ SceneSize.x / 2, 16 } }.draw(Arg::left = ColorF{ 1, 0 }, Arg::right = ColorF{ 1, alpha });
					RectF{ Arg::leftCenter = itemCenter + vibrate, SizeF{ SceneSize.x / 2, 16 } }.draw(Arg::left = ColorF{ 1, alpha }, Arg::right = ColorF{ 1, 0 });
				}

				const ColorF textColor = (selected && pauseMenu_.decideTimer().isRunning()) ?
					(LightYellow.lerp(Palette::Gray, Periodic::Square0_1(0.12s))) :
					(selected ? Palette::White : Palette::Silver);
				DrawText(U"px7812", item.text, Arg::center = itemCenter, textColor);
			}

			const Vec2 titleCenter = SceneRect.center().withY(LineHeight * 1);
			RectF{ Arg::center = titleCenter, SizeF{ SceneSize.x, 16 } }.draw(Palette::Black);
			DrawText(U"px7812", U"PAUSED - 休憩中", Arg::center = titleCenter, Palette::White);

			const Vec2 descCenter = SceneRect.center().withY(SceneSize.y - LineHeight * 1);
			RectF{ Arg::center = descCenter, SizeF{ SceneSize.x, 16 } }.draw(Palette::Black);
			DrawText(U"px7812", PauseMenuItemList[pauseMenu_.selectedIndex()].desc, Arg::center = descCenter, Palette::Silver);

			SceneRect.stretched(-4).drawFrame(2.0, Palette::Gray);
		}
	}

	void MainScene::onDestroyAllEnemies_()
	{
		if (not timeDestroyAllEnemy_.isRunning())
		{
			timeDestroyAllEnemy_.start();

			// クリアタイム更新
			ApplyStageClearRecord_();
		}
	}

	void MainScene::onGetKey_()
	{
		// プレイヤーが鍵をとった
		// ゲームクリア表示へ...
		timeGetKey_.start();

		// クリアタイム更新
		ApplyStageClearRecord_();
	}

	void MainScene::onGaugeMax_()
	{
		if (not getData().stageData->openedSecretRoute && not timerGaugeMax_.isStarted())
		{
			// スクロールロック解除(openedSecretRoute => true)はブロック破壊と同時に行う

			// ゲージマックス時ゲージエフェクト
			getData().gauge->startDrawMaxEffect();

			// 時間差でブロック破壊
			timerGaugeMax_.start();

			// SE
			PlayAudioOneShot(U"Unlock");
		}
	}

	void MainScene::onCameraShake_(const CameraShakeEvent& event)
	{
		cameraShakeIntensity_ = event.intensity;
		timerCameraShake_.restart(event.time);
	}

	void MainScene::onGetSpecialItem_(const GetSpecialItemEvent& event)
	{
		timerGetSpItem_.restart();
		gotSpItem_ = event.type;
	}

	void MainScene::onGetLastKey_()
	{
		if (not timeGetLastKey_.isRunning())
		{
			timeGetLastKey_.start();

			getData().player->startEndingSequence();

			// クリアタイム更新
			ApplyStageClearRecord_();
		}
	}

	void MainScene::ApplyStageClearRecord_()
	{
		stageClearTime_ = time_.sF();

		const auto stageID = getData().stageData->stageID;

		// レコードが存在しない場合、または新記録の場合に保存する
		if (not getData().currentRecords.contains(stageID) ||
			getData().currentRecords.at(stageID).timeSec > stageClearTime_)
		{
			getData().currentRecords[stageID] = StageRecord{ stageClearTime_ };
		}
	}
}

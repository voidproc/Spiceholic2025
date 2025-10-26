#include "MainScene.h"
#include "Actor/Block.h"
#include "Actor/Effect.h"
#include "Actor/Enemy.h"
#include "Actor/Item.h"
#include "Actor/Player.h"
#include "Actor/MakeActor.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/Color.h"
#include "Core/DrawText.h"
#include "Core/DrawSprite.h"
#include "Core/Gauge.h"
#include "Core/Score.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"
#include "Input/ActionInput.h"
#include "Stage/Stage.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	namespace
	{
		enum class PauseMenuItemType
		{
			Back,
			Retry,
			Title,
		};

		struct PauseMenuItem
		{
			PauseMenuItemType type;
			StringView text;
			StringView desc;
		};

		constexpr std::array<PauseMenuItem, 3> PauseMenuItemList = { {
			{ PauseMenuItemType::Back, U"Back to Game"_sv, U"ゲームに戻ります"_sv },
			{ PauseMenuItemType::Retry, U"Retry This Stage"_sv, U"このステージをやり直します"_sv },
			{ PauseMenuItemType::Title, U"Quit to Title"_sv, U"タイトル画面に戻ります"_sv },
		} };


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
				gameData.blocks.push_back(std::make_unique<Block>(spawn.position, spawn.type, gameData, spawn.bringItems, spawn.secretRoute));
				actor = gameData.blocks.back().get();
				break;

			case ActorType::ItemChilipepper:
			case ActorType::ItemKey:
				gameData.actors.push_back(std::make_unique<Item>(spawn.position, spawn.type, gameData));
				actor = gameData.actors.back().get();
				break;

			case ActorType::EnemyChick:
				gameData.actors.push_back(std::make_unique<EnemyChick>(spawn.position, gameData, spawn.direction, spawn.subType, spawn.bringItems));
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
				if (actor.tag() == ActorTag::Weapon && block->type() == ActorType::BlockSpike) continue;

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
	}

	MainScene::MainScene(const InitData& init)
		:
		CustomScene{ init },
		time_{ StartImmediately::No, Clock() },
		stageClearTime_{ 0 },
		timeStageStart_{ StartImmediately::Yes, Clock() },
		shadowPosList_{ Arg::reserve = 128 },
		timeDestroyAllEnemy_{ StartImmediately::No, Clock() },
		initialEnemyCount_{},
		initialKeyCount_{},
		timeGetKey_{ StartImmediately::No, Clock() },
		timeStageClear_{ StartImmediately::No, Clock() },
		timerGaugeMax_{ 0.50s, StartImmediately::No, Clock() },
		openedSecretRoute_{ false },
		selectedPauseMenuIndex_{ 0 },
		timePause_{ StartImmediately::No },
		timerPauseMenuDecide_{ 0.80s, StartImmediately::No },
		timerPauseMenuMoveCursor_{},
		cameraShakeIntensity_{ 1.0 },
		timerCameraShake_{ 0.08s, StartImmediately::No, Clock() }
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
		initialEnemyCount_ = EnemyCount(getData().actors);
		initialKeyCount_ = KeyCount(getData().actors);

		// ゲージ初期化
		getData().gauge = std::make_unique<Gauge>();

		// イベント購読
		GetDispatch().subscribe<GetKeyEvent, &MainScene::onGetKey_>(this);
		GetDispatch().subscribe<GaugeMaxEvent, &MainScene::onGaugeMax_>(this);
		GetDispatch().subscribe<CameraShakeEvent, &MainScene::onCameraShakeEvent_>(this);
	}

	MainScene::~MainScene()
	{
		GetDispatch().unsubscribe<GetKeyEvent>(this);
		GetDispatch().unsubscribe<GaugeMaxEvent>(this);
		GetDispatch().unsubscribe<CameraShakeEvent>(this);
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
		else if (timeGetKey_.isRunning() || timeDestroyAllEnemy_.isRunning())
		{
			// ◆ 敵全滅～ステージクリア表示～シーン遷移
			//    または
			//    鍵取得～ステージクリア表示～シーン遷移
			updateStageClear_();
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
			return;
		}

		// ※ステージ開始時と、鍵取得～シーン遷移まではポーズしない
		if (timeStageStart_.isRunning()) return;
		if (timeGetKey_.isRunning()) return;

		GlobalClock::Pause();

		selectedPauseMenuIndex_ = 0;
		timePause_.restart();

		//SE
		PlayAudioOneShot(U"Pause1");
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

		// 鍵取得 or 敵全滅 → 少し待ってからクリア表示
		if (const auto time = Max(timeGetKey_.elapsed(), timeDestroyAllEnemy_.elapsed());
			time > 1.5s)
		{
			timeStageClear_.start();
		}

		// ステージクリア時の入力待ち→シーン遷移
		if (timeStageClear_ > 1.5s)
		{
			if (getData().actionInput->down(Action::Decide))
			{
				// シーン遷移する前に今のスコアを保存しておく（元に戻す用）
				getData().stageStartScore = getData().score->currentScore();

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
	}

	void MainScene::updatePaused_()
	{
		// ◆ ポーズ中

		// メニュー選択、項目決定
		// 項目決定されていたら実行しない
		if (not timerPauseMenuDecide_.isRunning())
		{
			// メニュー選択
			if (getData().actionInput->down(Action::MoveUp))
			{
				selectedPauseMenuIndex_ = (selectedPauseMenuIndex_ + PauseMenuItemList.size() - 1) % PauseMenuItemList.size();

				// 描画用
				timerPauseMenuMoveCursor_.restart(0.1s);

				// SE
				PlayAudioOneShot(U"Select2");
			}
			else if (getData().actionInput->down(Action::MoveDown))
			{
				selectedPauseMenuIndex_ = (selectedPauseMenuIndex_ + 1) % PauseMenuItemList.size();

				// 描画用
				timerPauseMenuMoveCursor_.restart(0.1s);

				// SE
				PlayAudioOneShot(U"Select2");
			}

			// 項目決定
			if (getData().actionInput->down(Action::Decide))
			{
				timerPauseMenuDecide_.start();

				// SE
				PlayAudioOneShot(U"Decide1");
			}
		}

		if (timerPauseMenuDecide_.reachedZero())
		{
			timerPauseMenuDecide_.reset();

			if (const auto& selected = PauseMenuItemList[selectedPauseMenuIndex_];
				selected.type == PauseMenuItemType::Back)
			{
				// ゲームに戻る
			}
			else if (selected.type == PauseMenuItemType::Retry)
			{
				// このステージをやり直す
				getData().score->set(getData().stageStartScore, true);
				getData().nextStageID = getData().stageData->stageID;
				changeScene(U"MainScene", 0);
			}
			else if (selected.type == PauseMenuItemType::Title)
			{
				// タイトルに戻る
				changeScene(U"TitleScene", 0);
			}

			// ポーズ解除
			togglePause_();
		}
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
			if (actor->type() == ActorType::EnemyChick)
			{
				UpdateActorPos(*actor, blocks);
			}
		}

		// アクター同士の衝突判定
		CheckCollision(player, actors, blocks);

		// アクターの破棄
		actors.remove_if([](const auto& actor) { return not actor->active(); });
		blocks.remove_if([](const auto& block) { return not block->active(); });

		// 敵全滅判定
		if (EnemyCount(getData().actors) == 0 && initialEnemyCount_ != 0 && initialKeyCount_ == 0)
		{
			onDestroyAllEnemies_();
		}

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
		const auto camera = cameraScrollTransform_();

		const Vec2 shakeAmount = timerCameraShake_.isRunning() ? (2.0 * Vec2{ 1, 0 } * cameraShakeIntensity_ * Periodic::Sine1_1(0.04s, ClockTime()) * timerCameraShake_.progress1_0()) : Vec2{};
		const Transformer2D shake{ Mat3x2::Translate(shakeAmount) };

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

	Transformer2D MainScene::cameraScrollTransform_() const
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

		// ステージ名
		const ColorF textColor{ Palette::Seashell.lerp(Palette::Red, 0.1 * Periodic::Square0_1(0.25s, ClockTime())) };
		const ColorF shadowColor{ Palette::Crimson.lerp(Palette::Silver, 0.3), 0.8 - 0.1 * Periodic::Square0_1(0.25s, ClockTime()) };
		const String& text = U"ステージ {} {}"_fmt(getData().stageData->stageID, getData().appSetting->get().stageSubtitles[getData().stageData->stageID]);
		DrawText(U"px7812", text, Arg::center = regionHudU.center(), textColor, shadowColor);

		// ゲージ枠、ゲージ
		getData().gauge->draw();
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
		// 帯
		const double height = 24 * EaseInQuad(Saturate(timeStageClear_.sF() / 0.3));
		RectF{ Arg::center = SceneCenter, SizeF{ SceneSize.x, height } }.draw(ColorF{ Palette::Darkred, 0.9 });

		// テキスト
		const String text = U"{}  Clear!  ({:02d}:{:02d})"_fmt(getData().stageData->name, (int)(stageClearTime_) / 60, (int)stageClearTime_ % 60);
		const Vec2 textPos = SceneCenter + Vec2{ 400 * (1 - EaseOutExpo(Saturate(timeStageClear_.sF() / 1.0))), 0 };
		DrawText(U"px7812", text, Arg::center = textPos, LightYellow.lerp(Palette::Black, 0.2 * Periodic::Square0_1(0.25s, ClockTime())));

		// 入力待ち表示
		if (timeStageClear_ > 1.5s)
		{
			DrawSprite(*getData().appSetting, U"WhiteArrowDown", 0.5s, false, SceneCenter + Vec2{ SceneSize.x / 2 - 16 , 4 + 1 * Periodic::Square0_1(0.5s, ClockTime()) });
		}
	}

	void MainScene::drawPaused_() const
	{
		// 背景を暗くする
		SceneRect.draw(ColorF{ 0, 0.9 });

		constexpr int LineHeight = 22;

		{
			const double t = Saturate(timePause_.sF() / 0.25);
			const double ease = 0.6 * EaseOutBack(t) + 0.4 * EaseOutQuad(t);
			Transformer2D trans{ Mat3x2::Translate(0, 500 * (1 - ease)) };

			for (const auto [index, item] : Indexed(PauseMenuItemList))
			{
				const Vec2 itemCenter = SceneCenter + Vec2{ 0, ((PauseMenuItemList.size() - 1) / -2.0 + index) * LineHeight };
				const bool selected = (index == selectedPauseMenuIndex_);

				// 選択行
				if (selected)
				{
					const Vec2 vibrate{ 0, timerPauseMenuMoveCursor_.isRunning() * 2 * Periodic::Sine1_1(0.01s) };
					const double alpha = (0.2 + 0.2 * Periodic::Jump0_1(0.75s)) * (0.8 + 0.2 * Periodic::Square0_1(32ms));
					RectF{ Arg::rightCenter = itemCenter + vibrate, SizeF{ SceneSize.x / 2, 16 } }.draw(Arg::left = ColorF{ 1, 0 }, Arg::right = ColorF{ 1, alpha });
					RectF{ Arg::leftCenter = itemCenter + vibrate, SizeF{ SceneSize.x / 2, 16 } }.draw(Arg::left = ColorF{ 1, alpha }, Arg::right = ColorF{ 1, 0 });
				}

				const ColorF textColor = (selected && timerPauseMenuDecide_.isRunning()) ?
					(LightYellow.lerp(Palette::Gray, Periodic::Square0_1(0.12s))) :
					(selected ? Palette::White : Palette::Silver);
				DrawText(U"px7812", item.text, Arg::center = itemCenter, textColor);
			}

			const Vec2 titleCenter = SceneRect.center().withY(LineHeight * 1);
			RectF{ Arg::center = titleCenter, SizeF{ SceneSize.x, 16 } }.draw(Palette::Black);
			DrawText(U"px7812", U"PAUSED - 休憩中", Arg::center = titleCenter, Palette::White);

			const Vec2 descCenter = SceneRect.center().withY(SceneSize.y - LineHeight * 1);
			RectF{ Arg::center = descCenter, SizeF{ SceneSize.x, 16 } }.draw(Palette::Black);
			DrawText(U"px7812", PauseMenuItemList[selectedPauseMenuIndex_].desc, Arg::center = descCenter, Palette::Silver);

			SceneRect.stretched(-4).drawFrame(2.0, Palette::Gray);
		}
	}

	void MainScene::onDestroyAllEnemies_()
	{
		if (not timeDestroyAllEnemy_.isRunning())
		{
			timeDestroyAllEnemy_.start();

			// TODO: クリアタイム
			//...
			stageClearTime_ = time_.sF();
		}
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

	void MainScene::onCameraShakeEvent_(const CameraShakeEvent& event)
	{
		cameraShakeIntensity_ = event.intensity;
		timerCameraShake_.restart(event.time);
	}
}

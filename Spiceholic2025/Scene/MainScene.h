#pragma once

#include "CustomScene.h"
#include "Stage/Stage.h"
#include "Core/Menu.h"

namespace Spiceholic
{
	struct CameraShakeEvent;

	class MainScene : public CustomScene
	{
	public:
		inline static const String Name = U"MainScene";

		MainScene(const InitData& init);

		~MainScene() override;

		void update() override;

	private:
		void togglePause_();

		void updateStageStart_();

		void updateStageClear_();

		void updatePaused_();

		void updateMain_();
		void onTimerGaugeMax_();

		void updateSnow_();

		void makeCharacterShadows_();

	public:
		void draw() const override;

	private:
		void drawMain_() const;

		// プレイヤー位置に追従するカメラ
		Transformer2D cameraScrollTransform_() const;
		RectF smoothCameraRect_;

		void drawHUD_() const;

		void drawStageStart_() const;

		void drawStageClear_() const;

		void drawPaused_() const;

		// 敵が全滅したときに1回呼ばれる
		void onDestroyAllEnemies_();

		// 購読イベント
		void onGetKey_();

		// 購読イベント
		void onGaugeMax_();

		// 購読イベント
		void onCameraShakeEvent_(const CameraShakeEvent& event);

		// ステージプレイ時間
		Stopwatch time_;
		// クリアタイム
		double stageClearTime_;

		// ステージ開始時待機用
		Stopwatch timeStageStart_;

		// アクターの影
		Array<Vec2> shadowPosList_;

		// 敵全滅関係
		// 敵全滅時待機用
		Stopwatch timeDestroyAllEnemy_;
		// 初期配置の敵の数
		int initialEnemyCount_;
		// 初期配置に鍵がある
		int initialKeyCount_;

		// 鍵取得時待機用
		Stopwatch timeGetKey_;
		// ステージクリア表示→シーン遷移
		Stopwatch timeStageClear_;
		// クリア表示メニュー
		Menu clearMenu_;
		//size_t selectedClearMenuIndex_;

		// シークレット関連
		Timer timerGaugeMax_;

		// ポーズ関連
		Stopwatch timePause_;
		// ポーズメニュー
		Menu pauseMenu_;

		// 画面揺れ
		double cameraShakeIntensity_;
		Timer timerCameraShake_;

		// 雪
		struct Snow
		{
			Vec2 pos;
			double noisePos;
			bool active;
		};
		Array<Snow> snow_;
		PerlinNoise noise_;
		Timer timerSnow_;
	};
}

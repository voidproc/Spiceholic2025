#pragma once

#include "CustomScene.h"
#include "Stage/Stage.h"

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

		void makeCharacterShadows_();

	public:
		void draw() const override;

	private:
		void drawMain_() const;

		// プレイヤー位置に追従するカメラ
		Transformer2D cameraScrollTransform_() const;

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
		// 鍵取得時待機後のステージクリア表示→シーン遷移
		Stopwatch timeStageClear_;

		// シークレット関連
		Timer timerGaugeMax_;

		// ポーズメニュー
		size_t selectedPauseMenuIndex_;
		Stopwatch timePause_;
		Timer timerPauseMenuDecide_;
		Timer timerPauseMenuMoveCursor_;

		// 画面揺れ
		double cameraShakeIntensity_;
		Timer timerCameraShake_;

	};
}

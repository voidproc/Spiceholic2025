#pragma once

#include "CustomScene.h"
#include "Stage/Stage.h"

namespace Spiceholic
{
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
		Transformer2D cameraTransform_() const;

		void drawHUD_() const;

		void drawStageStart_() const;

		void drawStageClear_() const;

		void drawPaused_() const;

		// 購読イベント
		void onGetKey_();

		// 購読イベント
		void onGaugeMax_();

		// ステージプレイ時間
		Stopwatch time_;
		// クリアタイム
		double stageClearTime_;

		// ステージ開始時待機用
		Stopwatch timeStageStart_;

		// アクターの影
		Array<Vec2> shadowPosList_;

		// 鍵取得時待機用
		Stopwatch timeGetKey_;
		// 鍵取得時待機後のステージクリア表示→シーン遷移
		Stopwatch timeStageClear_;

		// シークレット関連
		Timer timerGaugeMax_;
		// シークレット関連
		bool openedSecretRoute_;
	};
}

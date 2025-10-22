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

		void draw() const override;

	private:
		void recoverGaugeAuto_();
		void makeCharacterShadows_();
		Transformer2D cameraTransform_() const;
		void onGetKey_();
		void onGaugeMax_();

		Stopwatch time_;
		double stageClearTime_;

		Stopwatch timeStartReady_;

		// 炎ゲージ自動回復用タイマー
		Timer timerGaugeRecovery_;

		// アクターの影
		Array<Vec2> shadowPosList_;

		// 鍵をとった
		Stopwatch timeGetKey_;
		Stopwatch timeStageClear_;

		// シークレット
		bool openedSecretRoute_;
	};
}

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

		void onGetKey_();

		Stopwatch time_;

		Stopwatch timeStartReady_;

		// 炎ゲージ自動回復用タイマー
		Timer timerGaugeRecovery_;

		// アクターの影
		Array<Vec2> shadowPosList_;

		// 鍵をとった
		Stopwatch timeGetKey_;
	};
}

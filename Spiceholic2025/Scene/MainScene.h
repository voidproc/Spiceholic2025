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
		StageData stageData_;
		Stopwatch time_;
	};
}

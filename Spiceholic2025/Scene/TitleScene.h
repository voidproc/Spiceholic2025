#pragma once

#include "CustomScene.h"

namespace Spiceholic
{
	class TitleScene : public CustomScene
	{
	public:
		inline static const String Name = U"TitleScene";

		TitleScene(const InitData& init);

		~TitleScene() override;

		void update() override;

		void draw() const override;

	private:
		Timer timerSceneChange_;
	};
}

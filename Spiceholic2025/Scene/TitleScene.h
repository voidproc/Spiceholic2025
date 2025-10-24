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
		void selectNext_();
		void selectPrevious_();

		Stopwatch time_;

		Stopwatch timeDecide_;

		//Timer timerSceneChange_;

		// 選択中の項目
		size_t selectedMenuIndex_;

		String randomCharaTexName_;

		// 描画用
		Timer timerMoveCursorLeft_;
		Timer timerMoveCursorRight_;
	};
}

#pragma once

#include "App.h"

namespace Spiceholic
{
	class OptionScene : public App::Scene
	{
	public:
		inline static const String Name = U"OptionScene";

		OptionScene(const InitData& init);

		~OptionScene() override;

		void update() override;

		void draw() const override;

	private:
		// 次の設定項目を選択
		void selectNext_();

		// 前の設定項目を選択
		void selectPrevious_();

		// 選択中の設定項目
		size_t selectedFieldIndex_;

		// 描画用
		Timer timerMoveCursor_;
		Timer timerChangeValue_;
		size_t valueChangedIndex_;
	};
}

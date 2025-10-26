#pragma once

#include "App.h"
#include "Core/Menu.h"

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
		Menu menu_;

		// 描画用
		Timer timerChangeValue_;
		size_t valueChangedIndex_;
	};
}

#pragma once

#include "CustomScene.h"

namespace Spiceholic
{
	class LogoScene : public CustomScene
	{
	public:
		inline static const String Name = U"LogoScene";

		LogoScene(const InitData& init);

		~LogoScene() override;

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
	};
}

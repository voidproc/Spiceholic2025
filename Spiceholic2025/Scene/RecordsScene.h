#pragma once

#include "CustomScene.h"
#include "Core/Menu.h"

namespace Spiceholic
{
	class RecordsScene : public CustomScene
	{
	public:
		inline static const String Name = U"RecordsScene";

		RecordsScene(const InitData& init);

		~RecordsScene() override;

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
	};
}

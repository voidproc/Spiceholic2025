#pragma once

#include "CustomScene.h"
#include "Core/Menu.h"

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
		Menu menu_;

		Stopwatch time_;
		Stopwatch timeDecide_;

		// キャラクタをランダムに変更
		String randomCharaTexName_;

		// 描画用
		Timer timerMoveCursorLeft_;
		Timer timerMoveCursorRight_;
		Stopwatch timeDescText_;

		// フェードイン
		Timer timerFadeIn_;

		struct BgObject
		{
			Vec2 pos;
			double scale;
			double angle;
		};
		Array<BgObject> bgObj_;
		Timer timerBgObj_;
	};
}

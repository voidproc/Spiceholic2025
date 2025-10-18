# include <Siv3D.hpp> // Siv3D v0.6.16

//#include "GameContext/GameData.h"
#include "Scene/Scenes.h"

namespace
{
	constexpr ColorF DefaultBgColor{ Palette::Black };

	void InitSivSystem()
	{
		Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

		// TODO: アプリ終了条件
		//...
	}

	void LoadImage()
	{
		TextureAsset::Register(U"Chara", Resource(U"image/chara.png"));
		TextureAsset::Register(U"GaugeFrame", Resource(U"image/gauge_frame.png"));
		TextureAsset::Register(U"Gauge", Resource(U"image/gauge.png"));
		TextureAsset::Register(U"Block", Resource(U"image/block.png"));

		//TextureAsset::Load();
	}
}

void Main()
{
	using namespace Spiceholic;

	InitSivSystem();

	// アセット読み込み
	LoadImage();
	//LoadFont();
	//LoadAudio();

	// ゲームデータ
	auto gameData = std::make_shared<GameData>();

	// TODO: 低解像度シーン
	//...

	// シーン初期化
	App app{ gameData };
	RegisterScenes<MainScene>(app);
	app.setFadeColor(DefaultBgColor);
	app.init(MainScene::Name, 0s);

	while (System::Update())
	{
		if (not app.updateScene())
		{
			break;
		}

		app.drawScene();
	}
}

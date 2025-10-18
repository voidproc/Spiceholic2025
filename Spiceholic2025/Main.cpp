# include <Siv3D.hpp> // Siv3D v0.6.16

namespace
{
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
	InitSivSystem();

	// アセット読み込み
	LoadImage();
	//LoadFont();
	//LoadAudio();

	// TODO: 低解像度シーン
	//...

	// TODO: シーン登録
	//...

	while (System::Update())
	{
		TextureAsset(U"Chara").draw();
	}
}

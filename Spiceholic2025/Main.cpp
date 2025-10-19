# include <Siv3D.hpp> // Siv3D v0.6.16

#include "Config/GameConfig.h"
#include "Core/LowResolution.h"
#include "Scene/Scenes.h"
#include "Setting/AppSetting.h"

namespace
{
	using namespace Spiceholic;

	constexpr ColorF DefaultBgColor{ Palette::Black };

	void InitSivSystem(const AppSetting& appSetting)
	{
		Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

		Window::SetTitle(Format(appSetting.get().title, U" - v"), appSetting.get().version);

		// TODO: アプリ終了条件
		//...
	}

	void LoadImage()
	{
		TextureAsset::Register(U"Block", Resource(U"image/block.png"));
		TextureAsset::Register(U"Chara", Resource(U"image/chara.png"));
		TextureAsset::Register(U"Gauge", Resource(U"image/gauge.png"));
		TextureAsset::Register(U"GaugeFrame", Resource(U"image/gauge_frame.png"));
		TextureAsset::Register(U"Item", Resource(U"image/item.png"));

		//Load
		//...
	}

	void LoadFont()
	{
		auto data_px7812 = std::make_unique<FontAssetData>();
		data_px7812->font = Font{ 12, Resource(U"font/px7812.ttf"), FontStyle::Bitmap };

		auto data_k8x12L = std::make_unique<FontAssetData>();
		data_k8x12L->font = Font{ 12, Resource(U"font/k8x12L.ttf"), FontStyle::Bitmap };

		// フォールバックフォントの設定
		data_px7812->font.addFallback(data_k8x12L->font);

		FontAsset::Register(U"px7812", std::move(data_px7812));
		FontAsset::Register(U"k8x12L", std::move(data_k8x12L));

		//Load
		//...
	}
}

void Main()
{
	using namespace Spiceholic;

	// ゲームデータ
	auto gameData = std::make_shared<GameData>();
	gameData->appSetting->load();

	InitSivSystem(*gameData->appSetting);

	// アセット読み込み
	LoadImage();
	LoadFont();
	//LoadAudio();

	// 低解像度シーン
	constexpr double DefaultWindowScale = 4;  // TODO: 設定ファイルからの読込
	LowResolution lowres{ SceneSize, DefaultWindowScale };

	// シーン初期化
	App app{ gameData };
	RegisterScenes<TitleScene, MainScene>(app);
	app.setFadeColor(DefaultBgColor);
	app.init(TitleScene::Name, 0s);

	while (System::Update())
	{
		if (not app.updateScene())
		{
			break;
		}

		{
			const auto target = lowres.scopedRenderTarget();
			app.drawScene();
		}

		lowres.draw();

		// TODO: ウィンドウスケール設定変更時に反映
		//...
	}
}

# include <Siv3D.hpp> // Siv3D v0.6.16

#include "Audio/AudioVolume.h"
#include "Config/GameConfig.h"
#include "Core/LowResolution.h"
#include "Event/Dispatch.h"
#include "Scene/Scenes.h"
#include "Setting/AppSetting.h"
#include "Setting/UserSetting.h"

namespace
{
	using namespace Spiceholic;

	constexpr ColorF DefaultBgColor{ Palette::Black };

	void InitSivSystem(const AppSetting& appSetting)
	{
		Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

#ifdef DEBUG_MODE
		Window::SetTitle(Format(appSetting.get().title, U" - v", appSetting.get().version, U" ★DEBUG_MODE"));
#else
		Window::SetTitle(Format(appSetting.get().title, U" - v", appSetting.get().version));
#endif

		// ESCキーで終了しない
		System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	}

	void LoadImage()
	{
		TextureAsset::Register(U"Block", Resource(U"image/block.png"));
		TextureAsset::Register(U"Chara", Resource(U"image/chara.png"));
		TextureAsset::Register(U"Gauge", Resource(U"image/gauge.png"));
		TextureAsset::Register(U"GaugeFrame", Resource(U"image/gauge_frame.png"));
		TextureAsset::Register(U"Item", Resource(U"image/item.png"));
		TextureAsset::Register(U"ItemKey", Resource(U"image/item_key.png"));
		TextureAsset::Register(U"BurningStart1", Resource(U"image/burning_start_1.png"));
		TextureAsset::Register(U"BurningLoop1", Resource(U"image/burning_loop_1.png"));
		TextureAsset::Register(U"BurningEnd1", Resource(U"image/burning_end_1.png"));
		TextureAsset::Register(U"Shadow", Resource(U"image/shadow.png"));
		TextureAsset::Register(U"WhiteArrow", Resource(U"image/white_arrow.png"));
		TextureAsset::Register(U"EnemyChick", Resource(U"image/enemy_chick.png"));
		TextureAsset::Register(U"EnemyChickFly", Resource(U"image/enemy_chick_fly.png"));
		TextureAsset::Register(U"DragonGirl", Resource(U"image/dragon_girl.png"));
		TextureAsset::Register(U"DragonGirl2", Resource(U"image/dragon_girl_2.png"));
		TextureAsset::Register(U"Ground1", Resource(U"image/ground_1.png"));
		TextureAsset::Register(U"Ground2", Resource(U"image/ground_2.png"));
		TextureAsset::Register(U"Ground3", Resource(U"image/ground_3.png"));
		TextureAsset::Register(U"Snow", Resource(U"image/snow.png"));
		TextureAsset::Register(U"Head", Resource(U"image/head.png"));
		TextureAsset::Register(U"Smoke", Resource(U"image/smoke.png"));
		TextureAsset::Register(U"Powerup", Resource(U"image/powerup.png"));
		TextureAsset::Register(U"Notice", Resource(U"image/notice.png"));

		//Load
		//...
	}

	void LoadFont()
	{
		auto data_px7812 = std::make_unique<FontAssetData>();
		data_px7812->font = Font{ 12, Resource(U"font/px7812.ttf"), FontStyle::Bitmap };

		auto data_px7812m = std::make_unique<FontAssetData>();
		data_px7812m->font = Font{ 12, Resource(U"font/px7812m.ttf"), FontStyle::Bitmap };

		auto data_k8x12L = std::make_unique<FontAssetData>();
		data_k8x12L->font = Font{ 12, Resource(U"font/k8x12L.ttf"), FontStyle::Bitmap };

		// フォールバックフォントの設定
		data_px7812->font.addFallback(data_k8x12L->font);

		FontAsset::Register(U"px7812", std::move(data_px7812));
		FontAsset::Register(U"px7812m", std::move(data_px7812m));
		FontAsset::Register(U"k8x12L", std::move(data_k8x12L));
		FontAsset::Register(U"NotJamSig21", 21, Resource(U"font/Not_Jam_Signature_21.ttf"), FontStyle::Bitmap);

		//Load
		//...
	}

	void LoadAudio()
	{
		AudioAsset::Register(U"Select1", Resource(U"audio/select1.wav"));
		AudioAsset::Register(U"Select2", Resource(U"audio/select2.wav"));
		AudioAsset::Register(U"Pick1", Resource(U"audio/pick1.wav"));
		AudioAsset::Register(U"Walk1", Resource(U"audio/walk1.wav"));
		AudioAsset::Register(U"Explosion1", Resource(U"audio/explosion1.wav"));
		AudioAsset::Register(U"Explosion2", Resource(U"audio/explosion2.wav"));
		AudioAsset::Register(U"Fire1", Resource(U"audio/fire1.wav"));
		AudioAsset::Register(U"Powerup1", Resource(U"audio/powerup.ogg"));
		AudioAsset::Register(U"Damage1", Resource(U"audio/damage1.wav"));
		AudioAsset::Register(U"Damage1Loop", Audio::Stream, Resource(U"audio/damage1_loop.ogg"), Loop::Yes);
		AudioAsset::Register(U"Decide1", Resource(U"audio/decide1.ogg"));
		AudioAsset::Register(U"Pause1", Resource(U"audio/pause1.wav"));
		AudioAsset::Register(U"Area1", Audio::Stream, Resource(U"audio/area1.ogg"), Loop::Yes);
		AudioAsset::Register(U"Area2", Audio::Stream, Resource(U"audio/area2.ogg"), Loop::Yes);
		AudioAsset::Register(U"Area4", Audio::Stream, Resource(U"audio/area4.ogg"), Loop::Yes);

		//Load
		//...
	}
}

void Main()
{
	using namespace Spiceholic;

	// Clock
	GlobalClock::Init();

	// Event
	GlobalDispatch::Init();

	// ゲームデータ
	auto gameData = std::make_shared<GameData>();
	gameData->appSetting->load();
	gameData->userSetting->load(UserSettingFilePath);

	SetAudioVolume(*gameData->userSetting);

#ifdef DEBUG_MODE
	gameData->nextStageID = gameData->appSetting->get().debug.startStageID;
#endif

	InitSivSystem(*gameData->appSetting);

	// アセット読み込み
	LoadImage();
	LoadFont();
	LoadAudio();

	// 低解像度シーン
	const double DefaultWindowScale = gameData->userSetting->get().windowScale;
	LowResolution lowres{ SceneSize, DefaultWindowScale };

	// シーン初期化
	App app{ gameData };
	RegisterScenes<TitleScene, OptionScene, InputSettingScene, MainScene>(app);
	app.setFadeColor(DefaultBgColor);
	app.init(TitleScene::Name, 0s);

	// ウィンドウスケール設定変更を監視
	int32 previousScale = gameData->userSetting->get().windowScale;

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

		// ウィンドウスケール設定変更時に反映
		if (const auto newScale = gameData->userSetting->get().windowScale;
			newScale != previousScale)
		{
			lowres.setScale(newScale);
			previousScale = newScale;
		}
	}
}

# include <Siv3D.hpp> // Siv3D v0.6.16

#include "Audio/AudioVolume.h"
#include "Config/GameConfig.h"
#include "Core/LowResolution.h"
#include "Core/StageRecord.h"
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
		TextureAsset::Register(U"GaugeKara"	, Resource(U"image/gauge_kara.png"));
		TextureAsset::Register(U"Item", Resource(U"image/item.png"));
		TextureAsset::Register(U"ItemKey", Resource(U"image/item_key.png"));
		TextureAsset::Register(U"ItemCurry", Resource(U"image/item_curry.png"));
		TextureAsset::Register(U"ItemHabanero", Resource(U"image/item_habanero.png"));
		TextureAsset::Register(U"ItemMapo", Resource(U"image/item_mapo.png"));
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
		TextureAsset::Register(U"Ground4", Resource(U"image/ground_4.png"));
		TextureAsset::Register(U"Snow", Resource(U"image/snow.png"));
		TextureAsset::Register(U"Sparks", Resource(U"image/sparks.png"));
		TextureAsset::Register(U"Head", Resource(U"image/head.png"));
		TextureAsset::Register(U"Smoke", Resource(U"image/smoke.png"));
		TextureAsset::Register(U"Powerup", Resource(U"image/powerup.png"));
		TextureAsset::Register(U"Notice", Resource(U"image/notice.png"));
		TextureAsset::Register(U"Wasabi", Resource(U"image/wasabi.png"));
		TextureAsset::Register(U"ChilipepperBig", Resource(U"image/chilipepper_big.png"));

		// 事前ロード
		for (auto&& asset : TextureAsset::Enumerate())
		{
			TextureAsset::Load(asset.first);
		}
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
		FontAsset::Register(U"m3x6", 16, Resource(U"font/m3x6.ttf"), FontStyle::Bitmap);

		// 事前ロード
		constexpr StringView PreloadTextAlNum = U"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()=-~^|@`[{]};+:*,<.>/?_"_sv;
		constexpr StringView PreloadTextJa = U"ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろわをんァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロワンヴ一戻直画面次進投稿作品制作出演子辛麻婆豆腐草原遺跡深部雪降山道山頂部火口地帯使用変更左右切替上下左右移動入力割当攻撃決定押効果音量閲覧設定終了"_sv;
		FontAsset::Load(U"px7812", PreloadTextAlNum);
		FontAsset::Load(U"px7812m", PreloadTextAlNum);
		FontAsset::Load(U"k8x12L", PreloadTextAlNum);
		FontAsset::Load(U"k8x12L", PreloadTextJa);
		FontAsset::Load(U"NotJamSig21", PreloadTextAlNum);
		FontAsset::Load(U"m3x6", PreloadTextAlNum);
	}

	void LoadAudio()
	{
		AudioAsset::Register(U"Select1", Resource(U"audio/select1.ogg"));
		AudioAsset::Register(U"Select2", Resource(U"audio/select2.ogg"));
		AudioAsset::Register(U"Pick1", Resource(U"audio/pick1.ogg"));
		AudioAsset::Register(U"Pick2", Resource(U"audio/pick2.ogg"));
		AudioAsset::Register(U"Walk1", Resource(U"audio/walk1.ogg"));
		AudioAsset::Register(U"Explosion1", Resource(U"audio/explosion1.ogg"));
		AudioAsset::Register(U"Explosion2", Resource(U"audio/explosion2.ogg"));
		AudioAsset::Register(U"Fire1", Resource(U"audio/fire1.ogg"));
		AudioAsset::Register(U"Powerup1", Resource(U"audio/powerup.ogg"));
		AudioAsset::Register(U"Decide1", Resource(U"audio/decide1.ogg"));
		AudioAsset::Register(U"Pause1", Resource(U"audio/pause1.ogg"));
		AudioAsset::Register(U"Spawn", Resource(U"audio/spawn.ogg"));
		AudioAsset::Register(U"Unlock", Resource(U"audio/unlock.ogg"));
		AudioAsset::Register(U"BgmGrass", Audio::Stream, Resource(U"audio/bgm_grass.ogg"), Loop::Yes);
		AudioAsset::Register(U"BgmSnow", Audio::Stream, Resource(U"audio/bgm_snow.ogg"), Loop::Yes);
		AudioAsset::Register(U"BgmMagma", Audio::Stream, Resource(U"audio/bgm_magma.ogg"), Loop::Yes);

		// 事前ロード
		for (auto&& asset : AudioAsset::Enumerate())
		{
			AudioAsset::Load(asset.first);
		}
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

	// クリア記録を読み込み
	LoadRecords(RecordsFilePath, *gameData);

	// 低解像度シーン
	const double DefaultWindowScale = gameData->userSetting->get().windowScale;
	LowResolution lowres{ SceneSize, DefaultWindowScale };

	// シーン初期化
	App app{ gameData };
	RegisterScenes<LogoScene, TitleScene, OptionScene, InputSettingScene, MainScene, RecordsScene>(app);
	app.setFadeColor(DefaultBgColor);
	app.init(LogoScene::Name, 0s);

	// ウィンドウスケール設定変更を監視
	int32 previousScale = gameData->userSetting->get().windowScale;

	// シェーダ（スキャンライン、RGBシフト）
	const PixelShader ps = HLSL{ Resource(U"shader/scanline.hlsl"), U"PS_Texture" };
	if (not ps)
	{
		Logger << U"PixelShaderの作成に失敗";
	}

#ifdef DEBUG_MODE
	if (gameData->appSetting->get().debug.wait == 1)
	{
		while (System::Update())
		{
			if (KeyEnter.down())
			{
				break;
			}
		}
	}
#endif

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

		{
			const bool useEffect = gameData->userSetting->get().useEffect && gameData->userSetting->get().windowScale >= 2;
			const ScopedCustomShader2D shader = useEffect ? ScopedCustomShader2D{ ps } : ScopedCustomShader2D{};
			lowres.draw();
		}

		// ウィンドウスケール設定変更時に反映
		if (const auto newScale = gameData->userSetting->get().windowScale;
			newScale != previousScale)
		{
			lowres.setScale(newScale);
			previousScale = newScale;
		}
	}
}

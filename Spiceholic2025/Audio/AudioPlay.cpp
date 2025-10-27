#include "AudioPlay.h"

namespace Spiceholic
{
	constexpr int AudioVoiceCountMax = 32;

	namespace
	{
		const Array<StringView> BgmList = {
			U"Area1"_sv,
			U"Area2"_sv,
			U"Area4"_sv,
		};
	}

	void PlayAudioOneShot(StringView assetName)
	{
		if (GlobalAudio::GetActiveVoiceCount() < AudioVoiceCountMax)
		{
			AudioAsset(assetName).stopAllShots();
			AudioAsset(assetName).playOneShot();
		}
	}

	void StopAllBgm()
	{
		for (const auto& bgm : BgmList)
		{
			AudioAsset(bgm).stop();
		}
	}

	void StopAllBgm(const Duration& fade)
	{
		for (const auto& bgm : BgmList)
		{
			AudioAsset(bgm).stop(fade);
		}
	}
}

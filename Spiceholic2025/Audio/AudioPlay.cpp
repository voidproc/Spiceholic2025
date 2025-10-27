#include "AudioPlay.h"

namespace Spiceholic
{
	constexpr int AudioVoiceCountMax = 32;

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
		AudioAsset(U"Area1").stop();
		AudioAsset(U"Area2").stop();
	}

	void StopAllBgm(const Duration& fade)
	{
		AudioAsset(U"Area1").stop(fade);
		AudioAsset(U"Area2").stop(fade);
	}
}

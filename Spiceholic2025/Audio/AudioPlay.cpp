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

	void StopAllBgm(const Duration& /*fade*/)
	{
		//...
	}
}

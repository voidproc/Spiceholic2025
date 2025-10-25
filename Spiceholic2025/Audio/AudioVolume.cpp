#include "AudioVolume.h"
#include "Setting/UserSetting.h"

namespace Spiceholic
{
	namespace
	{
		// オーディオの音量設定。SEはMixBus0, BGMはMixBus1での再生を想定
		void SetAudioVolume_(double seVolume, double bgmVolume)
		{
			GlobalAudio::BusSetVolume(MixBus0, seVolume);
			GlobalAudio::BusSetVolume(MixBus1, bgmVolume);
		}
	}

	void SetAudioVolume(const UserSetting& userSetting)
	{
		SetAudioVolume_(userSetting.get().seVolume * 0.01, userSetting.get().bgmVolume * 0.01);
	}
}

#include "AudioVolume.h"

namespace Spiceholic
{
	void SetAudioVolume(double seVolume, double bgmVolume)
	{
		GlobalAudio::BusSetVolume(MixBus0, seVolume);
		GlobalAudio::BusSetVolume(MixBus1, bgmVolume);
	}
}

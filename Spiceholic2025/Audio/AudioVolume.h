#pragma once

namespace Spiceholic
{
	// オーディオの音量設定。SEはMixBus0, BGMはMixBus1での再生を想定
	void SetAudioVolume(double seVolume, double bgmVolume);
}

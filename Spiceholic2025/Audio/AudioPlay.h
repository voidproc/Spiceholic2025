#pragma once

namespace Spiceholic
{
	void PlayAudioOneShot(StringView assetName);
	void StopAllBgm();
	void StopAllBgm(const Duration& fade);
}

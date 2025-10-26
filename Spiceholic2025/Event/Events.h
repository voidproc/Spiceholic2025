#pragma once

namespace Spiceholic
{
	struct GetKeyEvent
	{
	};

	struct GaugeMaxEvent
	{
	};

	struct CameraShakeEvent
	{
		double intensity = 1.0;
		Duration time = 0.08s;
	};
}

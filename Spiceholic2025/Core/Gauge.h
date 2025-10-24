#pragma once

namespace Spiceholic
{
	class Gauge
	{
	public:
		Gauge(int32 value = 30);

		int32 add(int32 value);

		void update();

		void draw() const;

		int32 getValue() const;

		double displayValue() const;

		bool isPoweredUp() const;

		void startDrawMaxEffect();

	private:
		int32 currentValue_;
		double displayValue_;
		Stopwatch timeMaxEffect_;
	};
}

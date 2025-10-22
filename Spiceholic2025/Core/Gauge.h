#pragma once

namespace Spiceholic
{
	class Gauge
	{
	public:
		Gauge(double value = 0.3);

		double add(double value);

		void update();

		void draw() const;

		double getValue() const;

		double displayValue() const;

	private:
		double currentValue_;
		double displayValue_;
	};
}

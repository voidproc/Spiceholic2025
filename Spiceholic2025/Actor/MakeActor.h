#pragma once

namespace Spiceholic
{
	void MakeSmokeFxN(Array<std::unique_ptr<Actor>>& actors, const Vec2& pos, int n, const std::pair<double, double>& diffuseX, const std::pair<double, double>& diffuseY, const std::pair<double, double>& scaleRange, double vel, double delay);
}

#include "MakeActor.h"
#include "Effect.h"

namespace Spiceholic
{
	void MakeSmokeFxN(Array<std::unique_ptr<Actor>>& actors, const Vec2& pos, int n, const std::pair<double, double>& diffuseX, const std::pair<double, double>& diffuseY, const std::pair<double, double>& scaleRange, double vel, double delay)
	{
		actors.push_back(std::make_unique<FxSmoke>(
			pos + Vec2{},
			nullptr,
			Random(scaleRange.first, scaleRange.second),
			vel,
			0.00));

		for (int i = 1; i < n; ++i)
		{
			actors.push_back(std::make_unique<FxSmoke>(
				pos + Vec2{ Random(diffuseX.first, diffuseX.second), Random(diffuseY.first, diffuseY.second) },
				nullptr,
				Random(scaleRange.first, scaleRange.second),
				vel,
				delay * i));
		}
	}
}

#include "MakeActor.h"
#include "Effect.h"
#include "Item.h"

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

	void MakeItems(const String& items, GameData& gameData, const Vec2& pos)
	{
		if (items == U"Key")
		{
			gameData.actors.push_back(std::make_unique<Item>(pos, ActorType::ItemKey, gameData, true));
		}
		else if (items.starts_with(U"Chilipepper"))
		{
			int nItems = Parse<int>(String{ items.back() });
			Array<std::pair<Vec2, double>> spawn; // pos, delay

			if (nItems == 1)
			{
				spawn.push_back(std::make_pair(pos, 0.0));
			}
			else
			{
				const double a = Random(Math::TwoPi);
				for (int i = 0; i < nItems; ++i)
				{
					spawn.push_back(std::make_pair(pos + Circular{ 12, a + 120_deg * i }, 0.06 * i));
				}
			}

			for (const auto& s : spawn)
			{
				gameData.actors.push_back(std::make_unique<Item>(s.first, ActorType::ItemChilipepper, gameData, true, s.second));
			}
		}
	}
}

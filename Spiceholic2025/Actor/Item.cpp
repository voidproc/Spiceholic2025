#include "Item.h"
#include "Actor/Player.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
		constexpr double DefaultItemCollisionSize = 16;
	}

	Item::Item(const Vec2& pos, ActorType itemType)
		:
		Actor{ pos },
		collision_{},
		type_{ itemType }
	{
		collision_.set(RectF{ Arg::center = Vec2{}, DefaultItemCollisionSize });
	}

	Item::~Item()
	{
	}

	void Item::draw() const
	{
		const Vec2 pos = position().currentPos() + Vec2{ 0, 1.0 * Periodic::Sine1_1(1.5s) };
		const ColorF color{ 1 };

		if (type() == ActorType::ItemChilipepper)
		{
			const double t = 1.0 - EaseInOutCubic(Periodic::Triangle0_1(0.5s)) * Periodic::Square0_1(1.0s);
			ScopedColorMul2D mul(t, t, t, 1);
			ScopedColorAdd2D add(1 - t, 1 - t, 1 - t, 0);

			TextureAsset(U"Item")(0, 0, 24).drawAt(pos, color);
		}
	}

	void Item::onCollide(Actor* /*other*/)
	{
		if (type() == ActorType::ItemChilipepper)
		{
			// なにかしらの効果（イベント発行など）
			//...
		}

		// 破棄待ちの状態へ
		setInactive();
	}

	const Vec2& Item::getCollisionPos() const
	{
		return position();
	}

	const Collision& Item::getCollision() const
	{
		return collision_;
	}

	int32 Item::score() const
	{
		int32 score = 0;

		if (type_ == ActorType::ItemChilipepper)
		{
			score = 100;
		}

		return score;
	}
}

#include "Block.h"
#include "Effect.h"
#include "Item.h"
#include "MakeActor.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
	}

	Block::Block(const Vec2& pos, ActorType type, GameData& gameData, const String& bringItems, bool secretRoute)
		:
		Actor{ pos },
		type_{ type },
		gameData_{ gameData },
		bringItems_{ bringItems },
		secretRoute_{ secretRoute },
		collision_{}
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 16 - 2 });
	}

	Block::~Block()
	{
	}

	void Block::update()
	{
	}

	void Block::draw() const
	{
		if (type() == ActorType::BlockSteel)
		{
			TextureAsset(U"Block")(0, 0, 16).drawAt(position());
		}
		else if (type() == ActorType::BlockCanBreak)
		{
			const double t = (timerDamaged_.isRunning()) ? Periodic::Square0_1(0.06s, ClockTime()) : 1;

			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1 - t, 0 };

			TextureAsset(U"Block")(16, 0, 16).drawAt(position());
		}
	}

	void Block::onCollide(Actor* other)
	{
	}

	void Block::onDead()
	{
		// 爆発
		explode();

		// アイテム放出
		MakeItems(bringItems_, gameData_, position().currentPos());
	}

	const Vec2& Block::getCollisionPos() const
	{
		return position();
	}

	const Collision& Block::getCollision() const
	{
		return collision_;
	}

	bool Block::invincible() const
	{
		return type() == ActorType::BlockSteel;
	}

	void Block::explode()
	{
		MakeSmokeFxN(gameData_.actors, position().currentPos(), 3, { 6,12 }, { -8,0 }, { 0.9,1.5 }, 0.3, 0.08);

		gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos(), secretRoute_));
	}

	void Block::setInactiveIfSecret()
	{
		if (secretRoute_)
		{
			setInactive(false);
			explode();
		}
	}
}

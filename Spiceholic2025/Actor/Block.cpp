#include "Block.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
	}

	Block::Block(const Vec2& pos, ActorType type)
		:
		Actor{ pos },
		type_{ type },
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
			TextureAsset(U"Block")(16, 0, 16).drawAt(position());
		}
	}

	const Vec2& Block::getCollisionPos() const
	{
		return position();
	}

	const Collision& Block::getCollision() const
	{
		return collision_;
	}

}

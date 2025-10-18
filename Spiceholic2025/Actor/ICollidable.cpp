#include "ICollidable.h"
#include "Core/Collision.h"

namespace Spiceholic
{
	bool ICollidable::isCollidingWith(const ICollidable& other) const
	{
		return getCollision().isCollidingWith(getCollisionPos(), other.getCollision(), other.getCollisionPos());
	}
}

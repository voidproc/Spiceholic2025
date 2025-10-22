#pragma once

namespace Spiceholic
{
	class Collision;

	class ICollidable
	{
	public:
		// 別のICollidableとのあたり判定結果を返す
		virtual bool isCollidingWith(const ICollidable& other) const;

		// あたり判定の中心位置を返す
		// このインターフェースを実装するコンポーネントがActor*を保持し、その pos() を返すなど
		virtual const Vec2& getCollisionPos() const = 0;

		// Collisionオブジェクトを返す
		// このインターフェースを実装するコンポーネントが保持する Collision を返すなど
		virtual const Collision& getCollision() const = 0;

	public:
		virtual ~ICollidable() = default;
	};

}

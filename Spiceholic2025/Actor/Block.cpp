#include "Block.h"
#include "Effect.h"
#include "Item.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
	}

	Block::Block(const Vec2& pos, ActorType type, GameData& gameData)
		:
		Actor{ pos },
		type_{ type },
		gameData_{ gameData },
		collision_{},
		timerDamaged_{ 0.3s, StartImmediately::No }
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
			const double t = (timerDamaged_.isRunning()) ? Periodic::Square0_1(0.06s) : 1;

			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1 - t, 0 };

			TextureAsset(U"Block")(16, 0, 16).drawAt(position());
		}
	}

	void Block::onCollide(Actor* other)
	{
		if (not active()) return;

		if (type() == ActorType::BlockCanBreak)
		{
			// 壊せるブロック
			if (other->tag() == ActorTag::Weapon)
			{
				// プレイヤーの攻撃で破壊された
				if (not timerDamaged_.isRunning())
				{
					timerDamaged_.restart();

					// 1回あたりダメージ
					setDamage(0.3);

					// 破壊されたらエフェクト、アイテム放出
					if (not active())
					{
						gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos()));

						// 仮: アイテム放出
						gameData_.actors.push_back(std::make_unique<Item>(position().currentPos(), ActorType::ItemChilipepper, gameData_, true));
					}
				}
			}
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

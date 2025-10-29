#include "Block.h"
#include "Effect.h"
#include "Item.h"
#include "MakeActor.h"
#include "Audio/AudioPlay.h"
#include "Config/GameConfig.h"
#include "Core/DrawSprite.h"
#include "Event/Dispatch.h"
#include "Event/Events.h"

namespace Spiceholic
{
	namespace
	{
	}

	Block::Block(const Vec2& pos, ActorType type, GameData& gameData, const String& bringItems, bool secretRoute, bool endingRoute)
		:
		Actor{ pos },
		type_{ type },
		gameData_{ gameData },
		bringItems_{ bringItems },
		secretRoute_{ secretRoute },
		endingRoute_{ endingRoute },
		collision_{}
	{
		collision_.set(RectF{ Arg::center = Vec2{}, 16 - 2 });
	}

	Block::~Block()
	{
	}

	void Block::update()
	{
		if (timerDamagedSE_.isRunning())
		{
			if (not AudioAsset(U"Damage1Loop").isPlaying())
			{
				AudioAsset(U"Damage1Loop").play();
			}
		}
		else
		{
			AudioAsset(U"Damage1Loop").stop();
		}
	}

	void Block::draw() const
	{
		if (type() == ActorType::BlockSteel)
		{
			TextureAsset(U"Block")(0, 0, 16).drawAt(position());
		}
		else if (type() == ActorType::BlockCanBreak ||
			     type() == ActorType::BlockCanBreakGray)
		{
			const double t = (timerDamaged_.isRunning()) ? Periodic::Square0_1(0.06s, ClockTime()) : 1;

			ScopedColorMul2D mul{ t, 1 };
			ScopedColorAdd2D add{ 1 - t, 0 };

			const int textureX = 16 + 16 * (type() == ActorType::BlockCanBreakGray);

			TextureAsset(U"Block")(textureX, 0, 16).drawAt(position());
		}
		else if (type() == ActorType::BlockSpike)
		{
			DrawSprite(*gameData_.appSetting, U"BlockSpike", 1s, false, position().currentPos());
		}
	}

	void Block::onCollide(Actor* /*other*/)
	{
	}

	void Block::onDead()
	{
		// 爆発
		explode();

		// アイテム放出
		MakeItems(bringItems_, gameData_, position().currentPos());

		// 画面揺れ
		GetDispatch().publish<CameraShakeEvent>({ 1.0, 0.08s });

		// SE
		PlayAudioOneShot(U"Explosion2");
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
		MakeSmokes(gameData_, position());

		gameData_.actors.push_back(std::make_unique<FxBlockBreak>(position().currentPos(), secretRoute_));
	}

	void Block::setInactiveIfSecretRoute()
	{
		if (secretRoute_)
		{
			setInactive(false);

			// 爆発
			explode();

			// 画面揺れ
			GetDispatch().publish<CameraShakeEvent>({ 1.5, 0.12s });

			// SE
			PlayAudioOneShot(U"Explosion2");
		}
	}

	void Block::setInactiveIfEndingRoute()
	{
		if (endingRoute_)
		{
			setInactive(false);

			// 爆発
			explode();

			// 画面揺れ
			GetDispatch().publish<CameraShakeEvent>({ 1.5, 0.12s });

			// SE
			PlayAudioOneShot(U"Explosion2");
		}
	}
}

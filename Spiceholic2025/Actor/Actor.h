#pragma once

#include "ICollidable.h"
#include "Core/Position.h"
#include "ActorTag.h"
#include "ActorType.h"

namespace Spiceholic
{
	class Actor : public ICollidable
	{
	public:
		static constexpr ActorTag Tag = ActorTag::None;
		static constexpr ActorType Type = ActorType::None;

	public:
		Actor(const Vec2& pos);

		virtual void update();

		virtual void draw() const;

		// 衝突時
		virtual void onCollide(Actor* /*actor*/);

		virtual void onDead();

		virtual ActorTag tag() const { return Tag; }
		virtual ActorType type() const { return Type; }

		// 位置
		const Position& position() const;

		// 位置を強制的に設定
		void setCurrentPosition(const Vec2& newPos);

		// このフレームの移動量を設定
		void setMoveAmount(const Vec2& moveAmount, bool apply = false);

		// このフレームの移動量
		const Vec2& getMoveAmount() const;

		// ユーザ入力に対応するX移動を反映する
		void applyMoveX(double ratio = 1.0);

		// ユーザ入力に対応するY移動を反映する
		void applyMoveY(double ratio = 1.0);

		// X移動反映を元に戻す
		void revertMoveX();

		// Y移動反映を元に戻す
		void revertMoveY();

		// 移動を確定する
		void confirmPosition();

		// オブジェクトが有効か
		bool active() const;

		// 破棄待ちの状態にする
		void setInactive(bool callOnDead = true);

		// ライフ
		double life() const;

		// ライフを設定
		void setLife(double value);

		// ダメージを受ける
		// ライフが0以下になったら非アクティブ（破棄待ちの状態）になる
		void setDamage(double value);

		virtual Vec2 shadowOffset() const
		{
			return Vec2{ 0, 8 };
		}

		virtual bool invincible() const
		{
			return true;
		}

		virtual int32 score() const
		{
			return 0;
		}

	private:
		Position pos_;
		Vec2 moveAmount_;
		bool active_;
		double life_;

	protected:
		Timer timerDamaged_;
	};

	// アクターどうしが衝突判定の対象かどうか
	bool IsCollidable(const Actor& actor1, const Actor& actor2);
}

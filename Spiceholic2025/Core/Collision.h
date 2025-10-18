#pragma once

namespace Spiceholic
{
	class Collision
	{
	public:
		using ShapesVariant = std::variant<RectF, Circle, Quad>;

		// あたり判定クラスを構築
		// 判定用の図形は大きさ 0 の RectF で初期化され、
		// このオブジェクトは無効な状態 (enabled() == false) となる
		[[nodiscard]]
		Collision();

		// あたり判定を設定する
		template <class Shape>
		void set(const Shape& shape)
		{
			collision_ = shape;
			enable();
		}

		// Collision同士が衝突しているか
		bool isCollidingWith(const Vec2& thisPos, const Collision& other, const Vec2& otherPos) const;

		// あたり判定が有効かどうか
		// 無効 (false) ならあたり判定チェックの対象とならない
		bool enabled() const noexcept;

		// あたり判定の有効／無効を設定する
		void enable(bool value = true) noexcept;

		// あたり判定を無効にする
		void disable() noexcept;

		// あたり判定を描画する
		void draw(const Vec2& pos) const;

	private:
		ShapesVariant collision_;
		bool enabled_;
	};
}

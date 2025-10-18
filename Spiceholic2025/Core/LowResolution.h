#pragma once

namespace Spiceholic
{
	/// 低解像度シーン描画用ユーティリティ
	/// ・低解像度シーン用のレンダーテクスチャ作成
	/// ・シーン／ウィンドウサイズ設定
	/// ・テクスチャ描画時フィルタとして ClampNearest を設定
	/// ・レンダーテクスチャをシーンのスケール値（コンストラクタ or setScale() で設定）で拡大して描画
	class LowResolution
	{
		Size sceneSize_;
		double scale_;
		ScopedRenderStates2D nearestSamplerState_;
		RenderTexture renderTexture_;

	public:
		/// @brief 低解像度シーン描画用のレンダーテクスチャを作成し、シーンの解像度とスケールを設定
		/// @param sceneSize シーンの解像度
		/// @param scale スケール（描画時にこの値で拡大される）
		LowResolution(const Size& sceneSize, double scale = 2.0);

		void setScale(double scale);

		double scale() const;

		ScopedRenderTarget2D scopedRenderTarget() const;

		// レンダーテクスチャの内容を Color{ 0, 0, 0, 255 } でクリアする
		void clear();

		// レンダーテクスチャをスケール値で拡大して描画する
		void draw() const;
	};
}

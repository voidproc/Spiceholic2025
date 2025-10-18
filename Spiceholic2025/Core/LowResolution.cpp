#include "LowResolution.h"

namespace Spiceholic
{
	LowResolution::LowResolution(const Size& sceneSize, double scale)
		:
		sceneSize_{ sceneSize },
		scale_{ scale },
		nearestSamplerState_{ SamplerState::ClampNearest },
		renderTexture_{ sceneSize, Palette::Black }
	{
		Scene::SetResizeMode(ResizeMode::Virtual);
		Window::Resize((sceneSize * scale).asPoint());
		Scene::SetTextureFilter(TextureFilter::Nearest);
	}

	void LowResolution::setScale(double scale)
	{
		if (AbsDiff(scale_, scale) > 1e-6)
		{
			scale_ = scale;
			Window::Resize((sceneSize_ * scale).asPoint());
		}
	}

	double LowResolution::scale() const
	{
		return scale_;
	}

	ScopedRenderTarget2D LowResolution::scopedRenderTarget() const
	{
		return ScopedRenderTarget2D{ renderTexture_ };
	}

	void LowResolution::clear()
	{
		renderTexture_.clear(Palette::Black);
	}

	void LowResolution::draw() const
	{
		const Transformer2D scale{ Mat3x2::Scale(scale_) };
		renderTexture_.draw();
	}
}

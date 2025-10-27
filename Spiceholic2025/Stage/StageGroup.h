#pragma once

namespace Spiceholic
{
	enum class StageGroupType : uint32
	{
		Grass,
		Grass2,
		Snow,
		Magma,
	};

	struct StageGroupInfo
	{
		StageGroupType group;
		String subtitle;
		String groundTexture;
		String bgm;
	};
}

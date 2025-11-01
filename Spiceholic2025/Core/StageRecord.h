#pragma once

namespace Spiceholic
{
	enum class ClearType : uint8
	{
		NoData = 0,
		Partial = 1,
		Cleared = 2,
	};

	struct StageRecord
	{
		double timeSec = 0;
		ClearType clearType = ClearType::NoData;

		template <class Archive>
		void SIV3D_SERIALIZE(Archive& archive)
		{
			archive(timeSec, clearType);
		}
	};

	StringView GetClearTypeName(ClearType type);
	void SaveRecords(FilePathView path, const GameData& gameData);
	void LoadRecords(FilePathView path, GameData& gameData);
}

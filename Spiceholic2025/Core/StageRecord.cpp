#include "StageRecord.h"
#include "Config/GameConfig.h"
#include "Core/StageRecord.h"
#include "Setting/AppSetting.h"

namespace Spiceholic
{
	StringView GetClearTypeName(ClearType type)
	{
		switch (type)
		{
		case ClearType::NoData: return U"(no data)"_sv;
		case ClearType::Partial: return U"Clear ?"_sv;
		case ClearType::Cleared: return U"Clear"_sv;
		}

		return U""_sv;
	}

	void SaveRecords(FilePathView path, const GameData& gameData)
	{
		Serializer<BinaryWriter> writer{ path };

		if (not writer)
		{
			Logger << U"クリア記録を保存できませんでした.";
			return;
		}

		writer(String{ U"VPBSH20251102" });

		for (const auto& stageID : gameData.appSetting->get().stageList)
		{
			if (gameData.currentRecords.contains(stageID))
			{
				writer(gameData.currentRecords.at(stageID));
			}
			else
			{
				writer(StageRecord{});
			}
		}
	}

	void LoadRecords(FilePathView path, GameData& gameData)
	{
		Deserializer<BinaryReader> reader{ path };

		if (not reader) return;

		String ver;
		reader(ver);

		if (ver != U"VPBSH20251102")
		{
			Logger << U"クリア記録が互換性のないバージョンであったため、読み込めませんでした.";
			return;
		}

		for (const auto& stageID : gameData.appSetting->get().stageList)
		{
			StageRecord rec;
			reader(rec);

			if (rec.clearType != ClearType::NoData)
			{
				gameData.currentRecords[stageID] = rec;
			}
		}
	}
}

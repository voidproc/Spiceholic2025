#include "Stage.h"
#include "ActorTypeFromTileID.h"
#include "Config/GameConfig.h"

namespace Spiceholic
{
	namespace
	{
		void MakeSpawnsFromTileLayer(const JSON& tileLayer, Array<ActorSpawnInfo>& spawns)
		{
			const int w = tileLayer[U"width"].get<int>();
			const auto data = tileLayer[U"data"];

			int index = 0;
			for (auto&& [key, tile] : data)
			{
				ActorSpawnInfo spawn{ .type = ActorType::None, .position = Vec2{}, .time = 0 };

				const int tileID = tile.get<int>();
				spawn.type = GetActorTypeFromTileID(tileID);

				const int x = index % w;
				const int y = index / w;
				spawn.position = Vec2{ (x + 0.5) * TileSize, (y + 0.5) * TileSize };

				spawn.time = 0;

				if ((spawn.type != ActorType::None) && (spawn.type != ActorType::PlayerA))
				{
					spawns.push_back(spawn);
				}

				++index;
			}
		}

		void MakeSpawnsFromObjectLayer(const JSON& objLayer, Array<ActorSpawnInfo>& spawns, Vec2& playerStartPos)
		{
			for (auto&& [keyObj, obj] : objLayer[U"objects"])
			{
				ActorSpawnInfo spawn{ .type = ActorType::None, .position = Vec2{}, .time = 0 };

				const int tileID = obj[U"gid"].get<int>();
				spawn.type = GetActorTypeFromTileID(tileID);

				const int x = obj[U"x"].get<int>();
				const int y = obj[U"y"].get<int>();
				spawn.position = Vec2{ x + TileSize / 2.0, y - TileSize / 2.0 };

				// プロパティ?
				if (obj.hasElement(U"properties"))
				{
					for (auto&& [keyProp, prop] : obj[U"properties"])
					{
						if (const String name = prop[U"name"].getString();
							name == U"SpawnTime")
						{
							spawn.time = prop[U"value"].getOr<double>(0.0);
						}
						else if (name == U"Life")
						{
							spawn.life = prop[U"value"].getOr<double>(1.0);
						}
					}
				}

				if (spawn.type == ActorType::PlayerA)
				{
					playerStartPos = spawn.position;
				}

				if ((spawn.type != ActorType::None) && (spawn.type != ActorType::PlayerA))
				{
					spawns.push_back(spawn);
				}
			}
		}
	}

	void LoadStage(const String& stageID, StageData& stageData)
	{
		auto stageJson = JSON::Load(Resource(U"stage/stage_{}.tmj"_fmt(stageID)));

		stageData.name = U"STAGE {}"_fmt(stageID);

		// StageName プロパティがあったら使う
		if (stageJson.hasElement(U"properties"))
		{
			for (auto&& [keyProp, prop] : stageJson[U"properties"])
			{
				if (const String name = prop[U"name"].getString();
					name == U"StageName")
				{
					stageData.name = prop[U"value"].getString();
				}
			}
		}

		stageData.gridSize.x = stageJson[U"width"].get<int>();
		stageData.gridSize.y = stageJson[U"height"].get<int>();

		if (stageJson[U"layers"].isEmpty()) return;

		for (auto&& [index, layer] : stageJson[U"layers"])
		{
			if (const String layerType = layer[U"type"].getString();
				layerType == U"tilelayer")
			{
				// タイルレイヤー
				MakeSpawnsFromTileLayer(layer, stageData.actorSpawns);
			}
			else if (layerType == U"objectgroup")
			{
				// オブジェクトレイヤー
				MakeSpawnsFromObjectLayer(layer, stageData.actorSpawns, stageData.playerStartPos);
			}
		}
	}
}

#pragma once

#include "world/core/WorldConfig.h"

#include <memory>
#include <map>
#include <utility>

#include "world/core/WorldTypes.h"
#include "ITerrainWorker.h"
#include "world/math/Perlin.h"
#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT PerlinTerrainGenerator : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    PerlinTerrainGenerator(int octaveCount = 5, double frequency = 8,
                           double persistence = 0.5);

    void setFrequency(double frequency);

    void setPersistence(double persistence);

    void setOctaveCount(int octaveCount);

    /** Perlin generator automatically adds octaves when generating bigger
     * lods. This parameter controls how many octaves the terrain can
     * have at maximum. 0 for unlimited.*/
    void setMaxOctaveCount(u32 maxOctaveCount);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void write(WorldFile &wf) const;

    void read(const WorldFile &wf);

    GridStorageBase *getStorage() override { return &_storage; }

    NodeCache *getCache() override { return &_cache; }

private:
    PerlinInfo _perlinInfo;
    Perlin _perlin;
    u32 _maxOctaves = 0;

    TerrainGrid _storage;
    NodeCache _cache;
    bool _seedUpdated = false;


    void processByNeighbours(Terrain &terrain, ITileContext &context);

    void processByTileCoords(Terrain &terrain, ITileContext &context);

    /** Ad-hoc method to read perlin seed from cache. Sets _seedUpdated to
     * false. */
    void updateSeed();
};
} // namespace world

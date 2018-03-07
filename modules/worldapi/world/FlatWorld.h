#pragma once
#include <worldapi/worldapidef.h>

#include "World.h"
#include "IWorldDecorator.h"
#include "../terrain/Ground.h"

class FlatWorld;

typedef IWorldDecorator<FlatWorld> IFlatWorldDecorator;

class PrivateFlatWorld;

class WORLDAPI_EXPORT FlatWorld : public World {
public:
	FlatWorld();
	~FlatWorld() override;

	void addFlatWorldDecorator(IFlatWorldDecorator *decorator);

	Ground& ground();
protected:
	virtual void generateChunk(Chunk & chunk);

private:
	PrivateFlatWorld * _internal;

	Ground _ground;
};

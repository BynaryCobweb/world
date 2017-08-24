#include "TerrainManipulator.h"

#include <armadillo/armadillo>

ITerrainManipulator * ITerrainManipulator::createManipulator() {
	return new TerrainManipulator();
}

void TerrainManipulator::setZ(Terrain & terrain, double x, double y, double value, int stage) const {
	if (stage == 0) {
		auto pos = terrain.getPixelPos(x, y);
		double oldValue = terrain._array(pos.x, pos.y);
		terrain._array(pos.x, pos.y) = value;
	}
	else {

	}
}

void TerrainManipulator::applyOffset(Terrain & terrain, const arma::mat & offset) const {
	if (offset.n_rows != terrain._array.n_rows || offset.n_cols != terrain._array.n_cols) {
		throw std::runtime_error("TerrainManipulator::applyOffset : bad matrix dimensions");
	}

	terrain._array += offset;
}

void TerrainManipulator::multiply(Terrain & terrain, const arma::mat & factor) const {
	if (factor.n_rows != terrain._array.n_rows || factor.n_cols != terrain._array.n_cols) {
		throw std::runtime_error("TerrainManipulator::multiply : bad matrix dimensions");
	}

	terrain._array %= factor;
}

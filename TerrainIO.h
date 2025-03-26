// TerrainIO.h
#pragma once
#include <string>
#include "Terrain.h"
bool ExportTerrainOBJ(const Terrain& terrain, const std::string& filename);
Terrain ImportTerrainOBJ(const std::string& filename);

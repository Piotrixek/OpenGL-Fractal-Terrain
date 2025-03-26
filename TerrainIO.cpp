// TerrainIO.cpp
#include "TerrainIO.h"
#include "Terrain.h"
#include <fstream>
#include <sstream>
#include <iostream>
bool ExportTerrainOBJ(const Terrain& terrain, const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Export failed: " << filename << "\n";
        return false;
    }
    const auto& vertices = terrain.getVertices();
    const auto& indices = terrain.getIndices();
    for (size_t i = 0; i < vertices.size(); i += 3) {
        ofs << "v " << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << "\n";
    }
    for (size_t i = 0; i < indices.size(); i += 3) {
        ofs << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "\n";
    }
    ofs.close();
    return true;
}
Terrain ImportTerrainOBJ(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Import failed: " << filename << "\n";
        return Terrain();
    }
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream iss(line.substr(2));
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (line.substr(0, 2) == "f ") {
            std::istringstream iss(line.substr(2));
            unsigned int a, b, c;
            iss >> a >> b >> c;
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }
    ifs.close();
    Terrain terrain;
    terrain.setVertices(vertices);
    terrain.setIndices(indices);
    return terrain;
}

// made by Piotrixek / Veni
// https://github.com/Piotrixek

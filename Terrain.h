// Updated Terrain.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
class Terrain {
public:
    Terrain(int size = 65, float scale = 1.0f, float heightScale = 20.0f);
    void generate();
    void draw();
    void drawSurface();
    void drawUnderground();
    void setUndergroundEnabled(bool enabled);
    int getSize() const;
    float getScale() const;
    float getHeightScale() const;
    const std::vector<float>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;
    void setVertices(const std::vector<float>& verts);
    void setIndices(const std::vector<unsigned int>& inds);
private:
    int size;
    float scale;
    float heightScale;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    std::vector<std::vector<float>> heightMap;
    bool undergroundEnabled;
    float undergroundDepth;
    unsigned int undergroundVAO, undergroundVBO, undergroundEBO;
    std::vector<float> undergroundVertices;
    std::vector<unsigned int> undergroundIndices;
    void diamondSquare();
    void setupMesh();
    void setupUnderground();
    void adjustSize();
};

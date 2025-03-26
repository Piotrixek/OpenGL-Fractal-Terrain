// Updated Terrain.cpp
#include "Terrain.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
Terrain::Terrain(int size, float scale, float heightScale) : size(size), scale(scale), heightScale(heightScale),
undergroundEnabled(true), undergroundDepth(10.0f), VAO(0), VBO(0), EBO(0),
undergroundVAO(0), undergroundVBO(0), undergroundEBO(0)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    adjustSize();
    heightMap.resize(this->size, std::vector<float>(this->size, 0.0f));
    generate();
}
void Terrain::adjustSize() {
    int s = size;
    int valid = s;
    if (s < 3) valid = 3;
    else {
        int n = s - 1;
        if ((n & (n - 1)) != 0) {
            int power = 1;
            while (power < n) power *= 2;
            valid = power + 1;
        }
    }
    if (valid != size) { size = valid; }
}
void Terrain::diamondSquare() {
    int step = size - 1;
    float roughness = 1.0f;
    heightMap[0][0] = ((float)rand() / RAND_MAX) * heightScale;
    heightMap[0][size - 1] = ((float)rand() / RAND_MAX) * heightScale;
    heightMap[size - 1][0] = ((float)rand() / RAND_MAX) * heightScale;
    heightMap[size - 1][size - 1] = ((float)rand() / RAND_MAX) * heightScale;
    while (step > 1) {
        int half = step / 2;
        for (int i = 0; i < size - 1; i += step) {
            for (int j = 0; j < size - 1; j += step) {
                float avg = (heightMap[i][j] + heightMap[i + step][j] + heightMap[i][j + step] + heightMap[i + step][j + step]) / 4.0f;
                heightMap[i + half][j + half] = avg + (((float)rand() / RAND_MAX) * 2 - 1) * roughness * step;
            }
        }
        for (int i = 0; i < size; i += half) {
            for (int j = (i + half) % step; j < size; j += step) {
                float sum = 0.0f;
                int count = 0;
                if (i - half >= 0) { sum += heightMap[i - half][j]; count++; }
                if (i + half < size) { sum += heightMap[i + half][j]; count++; }
                if (j - half >= 0) { sum += heightMap[i][j - half]; count++; }
                if (j + half < size) { sum += heightMap[i][j + half]; count++; }
                heightMap[i][j] = sum / count + (((float)rand() / RAND_MAX) * 2 - 1) * roughness * step;
            }
        }
        step /= 2;
        roughness *= 0.5f;
    }
}
void Terrain::generate() {
    adjustSize();
    heightMap.clear();
    heightMap.resize(size, std::vector<float>(size, 0.0f));
    diamondSquare();
    vertices.clear();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = (j - (size - 1) / 2.0f) * scale;
            float z = (i - (size - 1) / 2.0f) * scale;
            float y = heightMap[i][j];
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    indices.clear();
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1; j++) {
            int topLeft = i * size + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * size + j;
            int bottomRight = bottomLeft + 1;
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    setupMesh();
    if (undergroundEnabled)
        setupUnderground();
}
void Terrain::setupMesh() {
    if (VAO) { glDeleteVertexArrays(1, &VAO); }
    if (VBO) { glDeleteBuffers(1, &VBO); }
    if (EBO) { glDeleteBuffers(1, &EBO); }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
void Terrain::setupUnderground() {
    undergroundVertices.clear();
    undergroundIndices.clear();
    std::vector<glm::vec3> border;
    // Top row
    for (int j = 0; j < size; j++) {
        float x = (j - (size - 1) / 2.0f) * scale;
        float z = (0 - (size - 1) / 2.0f) * scale;
        float y = heightMap[0][j];
        border.push_back(glm::vec3(x, y, z));
    }
    // Right column
    for (int i = 1; i < size - 1; i++) {
        float x = ((size - 1) - (size - 1) / 2.0f) * scale;
        float z = (i - (size - 1) / 2.0f) * scale;
        float y = heightMap[i][size - 1];
        border.push_back(glm::vec3(x, y, z));
    }
    // Bottom row
    for (int j = size - 1; j >= 0; j--) {
        float x = (j - (size - 1) / 2.0f) * scale;
        float z = ((size - 1) - (size - 1) / 2.0f) * scale;
        float y = heightMap[size - 1][j];
        border.push_back(glm::vec3(x, y, z));
    }
    // Left column
    for (int i = size - 2; i > 0; i--) {
        float x = (0 - (size - 1) / 2.0f) * scale;
        float z = (i - (size - 1) / 2.0f) * scale;
        float y = heightMap[i][0];
        border.push_back(glm::vec3(x, y, z));
    }
    std::vector<glm::vec3> extruded;
    for (auto& v : border) {
        extruded.push_back(glm::vec3(v.x, v.y - undergroundDepth, v.z));
    }
    for (auto& v : border) {
        undergroundVertices.push_back(v.x);
        undergroundVertices.push_back(v.y);
        undergroundVertices.push_back(v.z);
    }
    for (auto& v : extruded) {
        undergroundVertices.push_back(v.x);
        undergroundVertices.push_back(v.y);
        undergroundVertices.push_back(v.z);
    }
    int n = border.size();
    for (int i = 0; i < n; i++) {
        int next = (i + 1) % n;
        int top1 = i;
        int top2 = next;
        int bottom1 = i + n;
        int bottom2 = next + n;
        undergroundIndices.push_back(top1);
        undergroundIndices.push_back(bottom1);
        undergroundIndices.push_back(top2);
        undergroundIndices.push_back(top2);
        undergroundIndices.push_back(bottom1);
        undergroundIndices.push_back(bottom2);
    }
    int centerIndex = undergroundVertices.size() / 3;
    glm::vec3 center(0.0f);
    for (int i = n; i < 2 * n; i++) {
        center += glm::vec3(undergroundVertices[i * 3 + 0], undergroundVertices[i * 3 + 1], undergroundVertices[i * 3 + 2]);
    }
    center /= n;
    undergroundVertices.push_back(center.x);
    undergroundVertices.push_back(center.y);
    undergroundVertices.push_back(center.z);
    for (int i = n; i < 2 * n; i++) {
        int next = (i + 1 < 2 * n) ? i + 1 : n;
        undergroundIndices.push_back(i);
        undergroundIndices.push_back(next);
        undergroundIndices.push_back(centerIndex);
    }
    if (undergroundVAO) { glDeleteVertexArrays(1, &undergroundVAO); }
    if (undergroundVBO) { glDeleteBuffers(1, &undergroundVBO); }
    if (undergroundEBO) { glDeleteBuffers(1, &undergroundEBO); }
    glGenVertexArrays(1, &undergroundVAO);
    glGenBuffers(1, &undergroundVBO);
    glGenBuffers(1, &undergroundEBO);
    glBindVertexArray(undergroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, undergroundVBO);
    glBufferData(GL_ARRAY_BUFFER, undergroundVertices.size() * sizeof(float), undergroundVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, undergroundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, undergroundIndices.size() * sizeof(unsigned int), undergroundIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
void Terrain::drawSurface() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
void Terrain::drawUnderground() {
    if (undergroundEnabled) {
        glBindVertexArray(undergroundVAO);
        glDrawElements(GL_TRIANGLES, undergroundIndices.size(), GL_UNSIGNED_INT, 0);
    }
}
void Terrain::draw() {
    drawSurface();
    if (undergroundEnabled)
        drawUnderground();
}
int Terrain::getSize() const { return size; }
float Terrain::getScale() const { return scale; }
float Terrain::getHeightScale() const { return heightScale; }
const std::vector<float>& Terrain::getVertices() const { return vertices; }
const std::vector<unsigned int>& Terrain::getIndices() const { return indices; }
void Terrain::setVertices(const std::vector<float>& verts) { vertices = verts; }
void Terrain::setIndices(const std::vector<unsigned int>& inds) { indices = inds; }
void Terrain::setUndergroundEnabled(bool enabled) {
    undergroundEnabled = enabled;
    generate();
}

// made by Piotrixek / Veni
// https://github.com/Piotrixek
// main.cpp
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.h"
#include "Logger.h"
#include "Shader.h"
#include "Callbacks.h"
#include "Globals.h"
#include "Terrain.h"
#include "TerrainIO.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fractal Terrain", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int skyVAO, skyVBO;
    float skyVertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int skyShader = loadShader("shaders/sky.vert", "shaders/sky.frag");
    if (!skyShader) {
        std::cerr << "Critical shader load error!\n";
        glfwTerminate();
        return -1;
    }
    unsigned int terrainShader = loadShader("shaders/terrain.vert", "shaders/terrain.frag");
    if (!terrainShader) {
        std::cerr << "Critical terrain shader load error!\n";
        glfwTerminate();
        return -1;
    }

    Terrain terrain;
    Logger logger;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    bool wireframe = false;
    bool undergroundToggle = true;
    glm::vec3 undergroundColor(1.0f, 0.0f, 1.0f);
    std::string objFilename = "terrain.obj";

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);
        glm::mat4 view = camera.getViewMatrix();

        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(skyShader);
        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(terrainShader);
        int projLoc = glGetUniformLocation(terrainShader, "projection");
        int viewLoc = glGetUniformLocation(terrainShader, "view");
        int modelLoc = glGetUniformLocation(terrainShader, "model");
        int undergroundLoc = glGetUniformLocation(terrainShader, "underground");
        int undergroundColorLoc = glGetUniformLocation(terrainShader, "undergroundColor");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniform1i(undergroundLoc, 0);
        terrain.drawSurface();
        glUniform1i(undergroundLoc, 1);
        glUniform3fv(undergroundColorLoc, 1, &undergroundColor[0]);
        terrain.drawUnderground();
        

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Terrain Controls");
        static int terrainSize = terrain.getSize();
        ImGui::SliderInt("Terrain Size (2^n+1)", &terrainSize, 33, 129);
        static float terrainScale = terrain.getScale();
        ImGui::SliderFloat("Terrain Scale", &terrainScale, 0.1f, 5.0f);
        static float terrainHeight = terrain.getHeightScale();
        ImGui::SliderFloat("Terrain Height", &terrainHeight, 5.0f, 50.0f);
        ImGui::Checkbox("Wireframe Mode", &wireframe);
        ImGui::Checkbox("Show Underground", &undergroundToggle);
        static float undergroundDepth = 10.0f;
        ImGui::SliderFloat("Underground Depth", &undergroundDepth, 5.0f, 30.0f);
        ImGui::ColorEdit3("Underground Color", &undergroundColor[0]);
        if (ImGui::Button("Regenerate Terrain")) {
            terrain = Terrain(terrainSize, terrainScale, terrainHeight);
            terrain.setUndergroundEnabled(undergroundToggle);
        }
        if (ImGui::Button("Export Terrain as OBJ")) {
            if (ExportTerrainOBJ(terrain, objFilename))
                logger.addLog("Exported terrain to " + objFilename);
            else
                logger.addLog("Export failed.");
        }
        if (ImGui::Button("Import Terrain from OBJ")) {
            terrain = ImportTerrainOBJ(objFilename);
            logger.addLog("Imported terrain from " + objFilename);
        }
        ImGui::End();
        logger.draw("Application Log");
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    glfwTerminate();
    return 0;
}

// made by Piotrixek / Veni
// https://github.com/Piotrixek
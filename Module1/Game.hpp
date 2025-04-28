#ifndef Game_hpp
#define Game_hpp
#pragma once

#include <entt/fwd.hpp>
#include "GameBase.h"
#include "RenderableMesh.hpp"
#include "ForwardRenderer.hpp"
#include "ShapeRenderer.hpp"

#include "Systems.hpp"
#include "Components.hpp"

/// @brief A Game may hold, update and render 3D geometry and GUI elements
class Game : public eeng::GameBase
{
public:
    /// @brief For game resource initialization
    /// @return 
    bool init() override;

    /// @brief General update method that is called each frame
    /// @param time Total time elapsed in seconds
    /// @param deltaTime Time elapsed since the last frame
    /// @param input Input from mouse, keyboard and controllers
    void update(
        float time,
        float deltaTime,
        InputManagerPtr input) override;

    /// @brief For rendering of game contents
    /// @param time Total time elapsed in seconds
    /// @param screenWidth Current width of the window in pixels
    /// @param screenHeight Current height of the window in pixels
    void render(
        float time,
        int windowWidth,
        int windowHeight) override;

    /// @brief For destruction of game resources
    void destroy() override;

private:
    /// @brief For rendering of GUI elements
    void renderUI();

    // Renderer for rendering imported animated or non-animated models
    eeng::ForwardRendererPtr forwardRenderer;

    // Immediate-mode renderer for basic 2D or 3D primitives
    ShapeRendererPtr shapeRenderer;

    // Entity registry - to use in labs
    std::shared_ptr<entt::registry> entity_registry;

    Systems systems;
    PointLight pointlight;

    // Matrices for view, projection and viewport
    struct Matrices
    {
        glm::mat4 V;
        glm::mat4 P;
        glm::mat4 VP;
        glm::ivec2 windowSize;
    } matrices;

    // Game meshes
    std::shared_ptr<eeng::RenderableMesh> grassMesh, horseMesh, characterMesh, foxMesh, playerMesh;

    // Game entity AABBs (for collision detection or visualization)
    eeng::AABB character_aabb1, character_aabb2, character_aabb3, horse_aabb, grass_aabb;

    // Placeholder animation state
    int characterAnimIndex = -1;
    float characterAnimSpeed = 1.0f;

    // Stats
    int drawcallCount = 0;

    /// @brief Placeholder system for updating the camera position based on inputs
    /// @param input Input from mouse, keyboard and controllers
    void updateCamera(
        InputManagerPtr input);

    void LoadMeshes();

    float gameTime = 0.0f;
};

#endif
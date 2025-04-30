#pragma once

#include "Components.hpp"

#include <memory>

#include <glmcommon.hpp>

#include <ForwardRenderer.hpp>
#include <ShapeRenderer.hpp>
#include <InputManager.hpp>

struct RenderDebug {
	bool showBones = false;
	bool showAABBs = true;
	bool showViewRays = true;
	bool showObjectBases = true;
};

struct Systems {
	RenderDebug debug; // Debug rendering options

	// Matrices for view, projection and viewport
	struct Matrices
	{
		glm::mat4 V;
		glm::mat4 P;
		glm::mat4 VP;
		glm::ivec2 windowSize;
	} matrices;

	void MovementSystem(entt::registry& reg, float deltaTime);

	void PlayerControllerSystem(entt::registry& reg, InputManagerPtr& input, float deltaTime);

	int RenderSystem(entt::registry& reg, int windowWidth, int windowHeight, eeng::ForwardRendererPtr forwardRenderer, ShapeRendererPtr shapeRenderer, float time);

	void NpcControllerSystem(entt::registry& reg);

	void CameraSystem(entt::registry& reg);

	void PointLightSystem(entt::registry& reg);
};
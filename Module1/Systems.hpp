#pragma once

#include "Components.hpp"

#include <memory>

#include <glmcommon.hpp>

#include <ForwardRenderer.hpp>
#include <ShapeRenderer.hpp>
#include <InputManager.hpp>


struct Systems {
	// Global system variables
	bool showBones = false;
	float keyDelay = 0.5f, keyTimer = 0.0f;

	void MovementSystem(entt::registry& reg, float deltaTime);

	void PlayerControllerSystem(entt::registry& reg, InputManagerPtr& input, float deltaTime);

	void RenderSystem(entt::registry& reg, eeng::ForwardRendererPtr forwardRenderer, ShapeRendererPtr shapeRenderer, float time);

	void NpcControllerSystem(entt::registry& reg);

	void CameraSystem(entt::registry& reg);

	void PointLightSystem(entt::registry& reg);
};
#pragma once

#include "Components.hpp"

#include <memory>

#include <glmcommon.hpp>

#include <ForwardRenderer.hpp>
#include <InputManager.hpp>


struct Systems {

	void MovementSystem(entt::registry& reg, float deltaTime);

	void PlayerControllerSystem(entt::registry& reg, InputManagerPtr& input);

	void RenderSystem(entt::registry& reg, eeng::ForwardRendererPtr forwardRenderer, float time);

	void NpcControllerSystem(entt::registry& reg);

	void CameraSystem(entt::registry& reg);

	void PointLightSystem(entt::registry& reg);
};
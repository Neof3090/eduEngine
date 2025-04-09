#pragma once

#include "Components.hpp"

#include <memory>

#include <glmcommon.hpp>

#include <ForwardRenderer.hpp>
#include <InputManager.hpp>


struct Systems {

	void MovementSystem(Transform& transforms, LinearVelocity& linearVelocities, float& deltaTime);

	void PlayerControllerSystem(Transform& transform, PlayerController& playerController, LinearVelocity& linearVelocity, InputManagerPtr& input, CameraComponent& camera);

	void RenderSystem(eeng::ForwardRendererPtr& forwardRenderer, const Transform& transform, const MeshComponent& mesh);

	void NpcControllerSystem(Transform& transform, LinearVelocity& linearVelocity, NpcController& npcCtrl);

	void CameraSystem(CameraComponent& camera, InputManagerPtr& input);

	void PointLightSystem(Transform& transforms, PointLight& pointLights);
};
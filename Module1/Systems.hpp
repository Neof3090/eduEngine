#pragma once

#include "Components.hpp"

void MovementSystem(std::shared_ptr<Transform> transforms, std::shared_ptr <LinearVelocity> linearVelocities);

void PlayerControllerSystem(std::shared_ptr<Transform> transforms, std::shared_ptr<PlayerController> playerControllers);

void RenderSystem(eeng::ForwardRendererPtr forwardRenderer, const Transform& transform, const Mesh& mesh);

void NpcControllerSystem(std::shared_ptr<Transform> transforms, std::shared_ptr<NpcController> npcControllers);

void PointLightSystem(std::shared_ptr<Transform> transforms, std::shared_ptr<PointLight> pointLights);

void CameraSystem(std::shared_ptr<Transform> transforms, std::shared_ptr<Camera> cameras);

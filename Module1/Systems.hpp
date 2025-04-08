#pragma once

#include "Components.hpp"

void MovementSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector < std::shared_ptr < LinearVelocity>> linearVelocities);

void PlayerControllerSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector<std::shared_ptr<PlayerController>> playerControllers);

void RenderSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector<std::shared_ptr<Mesh>> meshes);

void NpcControllerSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector<std::shared_ptr<NpcController>> npcControllers);

void PointLightSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector<std::shared_ptr<PointLight>> pointLights);

void CameraSystem(std::vector<std::shared_ptr<Transform>> transforms, std::vector<std::shared_ptr<Camera>> cameras);

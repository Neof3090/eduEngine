#include "Systems.hpp"

glm::mat4 GetWorldMatrix(Transform tfm) {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), tfm.position);

    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), tfm.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), tfm.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), tfm.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = rotationZ * rotationY * rotationX;

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), tfm.scale);

    // Combine into model matrix
    return translation * rotation * scale;
}

void Systems::MovementSystem(entt::registry& reg, float deltaTime) {
    // Update the position of the transform based on the linear velocity
    auto view = reg.view<Transform, LinearVelocity>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& linearVelocity = view.get<LinearVelocity>(entity);

        transform.position += linearVelocity.velocity * deltaTime;   
    }
}

void Systems::PlayerControllerSystem(entt::registry& reg, InputManagerPtr& input) {

    auto view = reg.view<Transform, LinearVelocity, PlayerController>();
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& linearVelocity = view.get<LinearVelocity>(entity);
        auto& playerController = view.get<PlayerController>(entity);
        auto& camera = reg.get<CameraComponent>(playerController.cameraEntity);

        // Update the player controller
        float speed = playerController.speed;
        glm::vec3 localMovement = glm::vec3(0.0f, 0.0f, 0.0f);

        // Check if any controller is connected
        if (/*input->GetConnectedControllerCount() > 0*/ false) {
            // Get the state of the first connected controller
            const auto& controllerState = input->GetControllerState(0);

            // Update linearVelocity based on controller input
            localMovement = glm::vec3(controllerState.axisLeftX, 0.0f, -controllerState.axisLeftY);

        }
        else {
            // Update linearVelocity based on keyboard input
            using Key = eeng::InputManager::Key;
            bool W = input->IsKeyPressed(Key::W);
            bool A = input->IsKeyPressed(Key::A);
            bool S = input->IsKeyPressed(Key::S);
            bool D = input->IsKeyPressed(Key::D);

            localMovement = glm::vec3((A ? -1.0f : 0.0f) + (D ? 1.0f : 0.0f), 0, (W ? 1.0f : 0.0f) + (S ? -1.0f : 0.0f));
        }

        // Normalize the velocity to ensure consistent speed
        if (glm::length(localMovement) > 0.0f)
            localMovement = glm::normalize(localMovement);
        else
            localMovement = glm::vec3(0.0f);

        // Fetch mouse and update camera controls
        auto mouse = input->GetMouseState();
        glm::ivec2 mouse_xy{ mouse.x, mouse.y };
        glm::ivec2 mouse_xy_diff{ 0, 0 };

        if (mouse.leftButton && playerController.mouse_xy_prev.x >= 0)
            mouse_xy_diff = playerController.mouse_xy_prev - mouse_xy;

        playerController.mouse_xy_prev = mouse_xy;

        camera.yaw += mouse_xy_diff.x * camera.sensitivity;
        camera.pitch += mouse_xy_diff.y * camera.sensitivity;
        camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

        // Update player rotation based on camera yaw (with offset because of model)
        transform.rotation.y = camera.yaw + glm::pi<float>();

        // Compute vectors in the local space of the player
        playerController.forward = glm::vec3(glm_aux::R(transform.rotation.y, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        playerController.right = glm::cross(playerController.forward, glm_aux::vec3_010);

        // Compute the total movement and set linearVelocity
        glm::vec3 worldMovement =
            -playerController.forward * localMovement.z +
            -playerController.right * localMovement.x;

        linearVelocity.velocity = worldMovement * speed;

        // Update player forward view ray
        playerController.viewRay = glm_aux::Ray{ transform.position + glm::vec3(0.0f, 2.0f, 0.0f), playerController.forward };

        // Update camera position
        float yOffset = 2.0f;
        camera.lookAt = transform.position + glm::vec3(0.0f, yOffset, 0.0f);

        // Update 
    }
}

void Systems::RenderSystem(entt::registry& reg, eeng::ForwardRendererPtr forwardRenderer, float time) {

    auto view = reg.view<Transform, MeshComponent>();
    for (auto entity : view) {
		auto& transform = view.get<Transform>(entity);
		auto& meshComp = view.get<MeshComponent>(entity);

        glm::mat4 modelToWorldMatrix = GetWorldMatrix(transform);

        meshComp.mesh->animate(meshComp.animIndex, time * meshComp.animSpeed);

        forwardRenderer->renderMesh(meshComp.mesh, modelToWorldMatrix);

        meshComp.aabb = meshComp.mesh->m_model_aabb.post_transform(modelToWorldMatrix);


        float axisLen = 25.0f;

        if (gizmo_bones) {
            for (int i = 0; i < characterMesh->boneMatrices.size(); ++i) {
                auto IBinverse = glm::inverse(characterMesh->m_bones[i].inversebind_tfm);
                glm::mat4 global = meh * characterMesh->boneMatrices[i] * IBinverse;
                glm::vec3 pos = glm::vec3(global[3]);

                glm::vec3 right = glm::vec3(global[0]); // X
                glm::vec3 up = glm::vec3(global[1]); // Y
                glm::vec3 fwd = glm::vec3(global[2]); // Z

                shapeRenderer->push_states(ShapeRendering::Color4u::Red);
                shapeRenderer->push_line(pos, pos + axisLen * right);

                shapeRenderer->push_states(ShapeRendering::Color4u::Green);
                shapeRenderer->push_line(pos, pos + axisLen * up);

                shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
                shapeRenderer->push_line(pos, pos + axisLen * fwd);

                shapeRenderer->pop_states<ShapeRendering::Color4u>();
                shapeRenderer->pop_states<ShapeRendering::Color4u>();
                shapeRenderer->pop_states<ShapeRendering::Color4u>();
            }
        }
	}
}

void Systems::NpcControllerSystem(entt::registry& reg) {

    auto view = reg.view<Transform, LinearVelocity, NpcController>();
    for (auto entity : view) {
		auto& transform = view.get<Transform>(entity);
		auto& linearVelocity = view.get<LinearVelocity>(entity);
		auto& npcCtrl = view.get<NpcController>(entity);

        glm::vec3 target = npcCtrl.patrolPoints[npcCtrl.currentPoint];
        float distance = glm::length(transform.position - target);

        if (distance < 0.1f) 
        {
            // Move to the next patrol point
            npcCtrl.currentPoint = (npcCtrl.currentPoint + 1) % npcCtrl.patrolPoints.size();
        }
        else 
        {
            linearVelocity.velocity = glm::normalize(target - transform.position) * npcCtrl.speed;
        }
	}
}

void Systems::CameraSystem(entt::registry& reg) {
    auto view = reg.view<Transform, CameraComponent>();
    for (auto entity : view) {
        auto& camera = view.get<CameraComponent>(entity);
        auto& transform = view.get<Transform>(entity);
        auto& targetTransform = reg.get<Transform>(camera.target);

        const glm::vec4 rotatedPos = glm_aux::R(camera.yaw, camera.pitch) * glm::vec4(0.0f, 0.0f, camera.distance, 1.0f);

        transform.position = camera.lookAt + glm::vec3(rotatedPos);
    }
};

void Systems::PointLightSystem(entt::registry& reg) {

}

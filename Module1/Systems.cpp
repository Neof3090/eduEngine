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

void Systems::MovementSystem(Transform& transform, LinearVelocity& linearVelocity, float& deltaTime) {
    // Update the position of the transform based on the linear velocity
    transform.position += linearVelocity.velocity * deltaTime;
}

void Systems::PlayerControllerSystem(Transform& transform, PlayerController& playerController, LinearVelocity& linearVelocity, InputManagerPtr& input, CameraComponent& camera) {

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

    // Compute vectors in the local space of the player
    playerController.forward = glm::vec3(glm_aux::R(camera.yaw, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    playerController.right = glm::cross(playerController.forward, glm_aux::vec3_010);

    // Compute the total movement and set linearVelocity
    glm::vec3 worldMovement =
        playerController.forward * localMovement.z +
        playerController.right * localMovement.x;

    linearVelocity.velocity = worldMovement * speed;

    // Update player forward view ray
    playerController.viewRay = glm_aux::Ray{ transform.position + glm::vec3(0.0f, 2.0f, 0.0f), playerController.forward };


    // Update camera to track the player
    camera.lookAt = transform.position + glm::vec3(0.0f, 2.0f, 0.0f);

}

void Systems::RenderSystem(eeng::ForwardRendererPtr& forwardRenderer, const Transform& transform, const MeshComponent& meshComponent) {

    glm::mat4 modelToWorldMatrix = GetWorldMatrix(transform);

    forwardRenderer->renderMesh(meshComponent.mesh, modelToWorldMatrix);
}

void Systems::NpcControllerSystem(Transform& transform, LinearVelocity& linearVelocity, NpcController& npcCtrl) {

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

void Systems::CameraSystem(CameraComponent& camera, InputManagerPtr& input) {
    // Fetch mouse and compute movement since last frame
    auto mouse = input->GetMouseState();
    glm::ivec2 mouse_xy{ mouse.x, mouse.y };
    glm::ivec2 mouse_xy_diff{ 0, 0 };
    if (mouse.leftButton && camera.mouse_xy_prev.x >= 0)
        mouse_xy_diff = camera.mouse_xy_prev - mouse_xy;
    camera.mouse_xy_prev = mouse_xy;

    // Update camera rotation from mouse movement
    camera.yaw += mouse_xy_diff.x * camera.sensitivity;
    camera.pitch += mouse_xy_diff.y * camera.sensitivity;
    camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

    // Update camera position
    const glm::vec4 rotatedPos = glm_aux::R(camera.yaw, camera.pitch) * glm::vec4(0.0f, 0.0f, camera.distance, 1.0f);
    camera.pos = camera.lookAt + glm::vec3(rotatedPos);
};

void Systems::PointLightSystem(Transform& transform, PointLight& pointLight) {

}

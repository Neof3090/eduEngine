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

void Systems::PlayerControllerSystem(entt::registry& reg, InputManagerPtr& input, float deltaTime) {

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
            bool SPACE = input->IsKeyPressed(Key::Space);

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

        
    }
}

int Systems::RenderSystem(entt::registry& reg, int windowWidth, int windowHeight, eeng::ForwardRendererPtr forwardRenderer, ShapeRendererPtr shapeRenderer, float time)
{
    // --- CAMERA SELECTION ---
    auto camView = reg.view<Transform, CameraComponent>();
    if (camView.begin() == camView.end()) {
        return 0; 
    }

    glm::vec3 cameraPos = glm::vec3(0.0f);

    for (auto entity : camView) {
        const auto& cam = camView.get<CameraComponent>(entity);
        const auto& camTfm = camView.get<Transform>(entity);

        cameraPos = camTfm.position;
        float aspect = float(windowWidth) / windowHeight;
        matrices.P = glm::perspective(glm::radians(60.0f), aspect, cam.nearPlane, cam.farPlane);
        matrices.V = glm::lookAt(cameraPos, cam.lookAt, cam.up);
        break;
    }


    // --- LIGHT SELECTION ---
    auto lightView = reg.view<PointLight>();
    glm::vec3 lightPos = glm::vec3(0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f);

    if (!lightView.empty()) {
        entt::entity pointlightEntity = lightView.front();
        auto& pointlight = reg.get<PointLight>(pointlightEntity);
        lightPos = pointlight.pos;
        lightColor = pointlight.color;
    }

    // --- BEGIN FORWARD PASS ---
    matrices.VP = glm_aux::create_viewport_matrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);
    forwardRenderer->beginPass(matrices.P, matrices.V, lightPos, lightColor, cameraPos);

    // --- RENDERING MESHES ---
    auto view = reg.view<Transform, MeshComponent>();
    for (auto entity : view) {
		auto& transform = view.get<Transform>(entity);
		auto& meshComp = view.get<MeshComponent>(entity);

        glm::mat4 modelToWorldMatrix = GetWorldMatrix(transform);

        if (meshComp.animIndex >= 0 && meshComp.animIndex < meshComp.mesh->getNbrAnimations()) 
        {
            meshComp.mesh->animate(meshComp.animIndex, time * meshComp.animSpeed);
        }

        forwardRenderer->renderMesh(meshComp.mesh, modelToWorldMatrix);

        meshComp.aabb = meshComp.mesh->m_model_aabb.post_transform(modelToWorldMatrix);

        meshComp.isSkinned = !meshComp.mesh->m_bones.empty();
        if (debug.showBones &&
            meshComp.isSkinned &&
            meshComp.mesh->boneMatrices.size() >= meshComp.mesh->m_bones.size() &&
            !meshComp.mesh->m_bones.empty())
        {
            float axisLen = transform.scale.y * 100.0f;
            
            size_t boneCount = std::min(meshComp.mesh->boneMatrices.size(), meshComp.mesh->m_bones.size());
            for (size_t i = 0; i < boneCount; ++i) {
                auto IBinverse = glm::inverse(meshComp.mesh->m_bones[i].inversebind_tfm);
                glm::mat4 global = GetWorldMatrix(transform) * meshComp.mesh->boneMatrices[i] * IBinverse;
                glm::vec3 pos = glm::vec3(global[3]);

                glm::vec3 right = glm::vec3(global[0]); // X
                glm::vec3 up = glm::vec3(global[1]);    // Y
                glm::vec3 fwd = glm::vec3(global[2]);   // Z

                shapeRenderer->push_states(ShapeRendering::Color4u::Red);
                shapeRenderer->push_line(pos, pos + axisLen * right);
                shapeRenderer->pop_states<ShapeRendering::Color4u>();

                shapeRenderer->push_states(ShapeRendering::Color4u::Green);
                shapeRenderer->push_line(pos, pos + axisLen * up);
                shapeRenderer->pop_states<ShapeRendering::Color4u>();

                shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
                shapeRenderer->push_line(pos, pos + axisLen * fwd);
                shapeRenderer->pop_states<ShapeRendering::Color4u>();
            }
        }

	}

    // --- END FORWARD PASS ---
    int drawcalls = forwardRenderer->endPass();

    if (debug.showObjectBases) {
        auto view = reg.view<Transform>();
        for (auto entity : view) {
            auto& tfm = view.get<Transform>(entity);
            glm::mat4 model = GetWorldMatrix(tfm);
            shapeRenderer->push_basis_basic(model, 1.0f);
        }
    }

    if (debug.showAABBs) {
        auto aabbView = reg.view<Transform, MeshComponent>();
        for (auto entity : aabbView) {
            auto& meshComp = aabbView.get<MeshComponent>(entity);

            shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
            shapeRenderer->push_AABB(meshComp.aabb.min, meshComp.aabb.max);
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
        }
    }

    if (debug.showViewRays) {
        auto playerView = reg.view<Transform, PlayerController>();
        for (auto entity : playerView) {
            auto& transform = playerView.get<Transform>(entity);
            auto& controller = playerView.get<PlayerController>(entity);
            const auto& ray = controller.viewRay;

            // Forward view ray
            glm::vec3 end = ray.origin + ray.dir * 100.0f;
            shapeRenderer->push_states(ShapeRendering::Color4u{ 0xffffffff });
            shapeRenderer->push_line(ray.origin, end);
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
        }
    }
    
    // --- SHAPE DEBUG FLUSH ---
    shapeRenderer->render(matrices.P * matrices.V);
    shapeRenderer->post_render();

    return drawcalls;
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

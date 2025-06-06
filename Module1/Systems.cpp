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
        auto& lv = view.get<LinearVelocity>(entity);
        auto& pc = view.get<PlayerController>(entity);
        auto& camera = reg.get<CameraComponent>(pc.cameraEntity);

        // Update the player controller
        float speed = pc.speed;
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
            bool LShift = input->IsKeyPressed(Key::LeftShift);
            bool SPACE = input->IsKeyPressed(Key::Space);

            // Jump logic
            if (pc.isGrounded) {
                // zero vertical velocity when on the ground
                lv.velocity.y = 0.0f;

                // start jump
                if (SPACE) {
                    pc.isGrounded = false;
                    pc.jumpTimer = 0.0f;

                    float halfDur = pc.jumpDelay * 0.5f;
                    pc.jumpVelocity = pc.jumpHeight / halfDur;

                    // immediately launch upward:
                    lv.velocity.y = pc.jumpVelocity;
                }
            }
            else {
                // in mid-air
                pc.jumpTimer += deltaTime;
                float T = pc.jumpDelay;
                float t = glm::min(pc.jumpTimer, T);
                float peakV = pc.jumpHeight * glm::pi<float>() / T;

                // Cheeky cosine function to simulate a jump arc
                lv.velocity.y = peakV * glm::cos(glm::pi<float>() * (t / T));

                if (pc.jumpTimer >= T) {
                    // landed
                    pc.isGrounded = true;
                    lv.velocity.y = 0.0f;
                }
            }

            // Set the speed based on the key pressed
            if (LShift) {
				speed = pc.runSpeed;
			}

            // Movement logic
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

        if (mouse.leftButton && pc.mouse_xy_prev.x >= 0)
            mouse_xy_diff = pc.mouse_xy_prev - mouse_xy;

        pc.mouse_xy_prev = mouse_xy;

        camera.yaw += mouse_xy_diff.x * camera.sensitivity;
        camera.pitch += mouse_xy_diff.y * camera.sensitivity;
        camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

        // Update player rotation based on camera yaw (with offset because of model)
        transform.rotation.y = camera.yaw + glm::pi<float>();

        // Compute vectors in the local space of the player
        pc.forward = glm::vec3(glm_aux::R(transform.rotation.y, glm_aux::vec3_010) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        pc.right = glm::cross(pc.forward, glm_aux::vec3_010);

        // Invert the forward and right vectors to match the camera's view direction
        pc.forward = -pc.forward;
        pc.right = -pc.right;

        // Compute the total movement and set linearVelocity
        glm::vec3 worldMovement =
            pc.forward * localMovement.z +
            pc.right * localMovement.x;

        glm::vec3 horizontalVel = worldMovement * speed;
        // preserve vertical velocity
        lv.velocity.x = horizontalVel.x;
        lv.velocity.z = horizontalVel.z;

        // Update camera position
        float yOffset = 2.0f;
        camera.lookAt = transform.position + glm::vec3(0.0f, yOffset, 0.0f);

        // Update player forward view ray
        pc.viewRay = glm_aux::Ray{ transform.position + glm::vec3(0.0f, 2.0f, 0.0f), pc.forward };

        // Intersect player view ray with AABBs of other objects 
        /* auto rayView = entity_registry->view<PlayerController>();
        for (auto entity : rayView) {
            auto& player = rayView.get<PlayerController>(entity);

            glm_aux::intersect_ray_AABB(player.viewRay, character_aabb2.min, character_aabb2.max);
            glm_aux::intersect_ray_AABB(player.viewRay, character_aabb3.min, character_aabb3.max);
            glm_aux::intersect_ray_AABB(player.viewRay, horse_aabb.min, horse_aabb.max);
        }*/

        // We can also compute a ray from the current mouse position,
        // to use for object picking and such ...
        //if (input->GetMouseState().rightButton)
        //{
        //    glm::ivec2 windowPos(camera.mouse_xy_prev.x, matrices.windowSize.y - camera.mouse_xy_prev.y);
        //    auto ray = glm_aux::world_ray_from_window_coords(windowPos, matrices.V, matrices.P, matrices.VP);
        //    // Intersect with e.g. AABBs ...

        //    eeng::Log("Picking ray origin = %s, dir = %s",
        //        glm_aux::to_string(ray.origin).c_str(),
        //        glm_aux::to_string(ray.dir).c_str());
        //}

    }
}

void Systems::PlayerAnimationSystem(entt::registry& reg, InputManagerPtr& input, float deltaTime) {
    auto view = reg.view<PlayerController, LinearVelocity, AnimationComponent>();
    for (auto entity : view) {
        auto& pc = view.get<PlayerController>(entity);
        auto& lv = view.get<LinearVelocity>(entity);
        auto& ac = view.get<AnimationComponent>(entity);

        if (ac.manualBlending) {
            ac.index = 1; // Idle animation
            ac.nextIndex = 3; // Sprint animation
            continue;
        }

        auto xzVel = glm::length(glm::vec2(lv.velocity.x, lv.velocity.z));
        float yVel = lv.velocity.y;
        float runThreshold = pc.runSpeed-0.1;

        AnimationComponent::State prevState = ac.state;
        if (!pc.isGrounded) {
            ac.state = AnimationComponent::State::Jump;
        }
        else if (xzVel >= runThreshold) {
            ac.state = AnimationComponent::State::Run;
        }
        else if (xzVel > 0.1f) {
            ac.state = AnimationComponent::State::Walk;
		}
		else {
			ac.state = AnimationComponent::State::Idle;
		}

        int desiredIndex; 

        // simple FSM example
        switch (ac.state) {
        case AnimationComponent::State::Idle:
            desiredIndex = 1; // Idle animation
            
            // Example: set looping animation options
            break;
        case AnimationComponent::State::Walk:
            desiredIndex = 2; // Walk animation
            break;
        case AnimationComponent::State::Run:
            desiredIndex = 3; // Sprint animation
            break;
        case AnimationComponent::State::Jump:
            desiredIndex = 4; // Jump animation
            break;
        default:
            desiredIndex = 0; // Default to T-pose
	    break;
        }

        if (ac.queue.empty()) 
        {
            ac.queue.push(desiredIndex);
        } 
        else if (desiredIndex != ac.nextIndex && ac.queue.back() != desiredIndex && ac.queue.size() < 1) 
        {
            ac.queue.push(desiredIndex);
        }
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

        forwardRenderer->renderMesh(meshComp.mesh, modelToWorldMatrix);

        meshComp.aabb = meshComp.mesh->m_model_aabb.post_transform(modelToWorldMatrix);

        meshComp.isSkinned = !meshComp.mesh->m_bones.empty();
        if (debug.showBones &&
            meshComp.isSkinned &&
            meshComp.mesh->boneMatrices.size() >= meshComp.mesh->m_bones.size() &&
            !meshComp.mesh->m_bones.empty())
        {
            float axisLen = 0.5f / transform.scale.y;
            
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
    if (debug.showAABBs || debug.showBones || debug.showObjectBases ||debug.showViewRays)
    shapeRenderer->render(matrices.P * matrices.V);
    shapeRenderer->post_render();

    return drawcalls;
}

void Systems::AnimationSystem(entt::registry& reg, float dt) {
    auto animView = reg.view<MeshComponent, AnimationComponent>();
    for (auto entity : animView) {
        auto& mc = animView.get<MeshComponent>(entity);
        auto& ac = animView.get<AnimationComponent>(entity);

        // advance both playheads
        ac.timeCur += dt * ac.speed;
        ac.timeNext += dt * ac.speed;

        // Manual blending
        if (ac.manualBlending) {

            mc.mesh->animateBlend(
                ac.index, ac.nextIndex,
                ac.timeCur, ac.timeNext,
                ac.blendFrac,
                eeng::AnmationTimeFormat::NormalizedTime,
                eeng::AnmationTimeFormat::NormalizedTime
            );
            continue;
        }

        // Check if we need to dequeue the next animation
        if (ac.index == ac.nextIndex && !ac.queue.empty()) {
            ac.nextIndex = ac.queue.front();
            ac.queue.pop();
            ac.timeNext = 0.0f;
            ac.blendFrac = 0.0f;
        }

        // Are we cross-fading?
        if (ac.index != ac.nextIndex) {

            ac.blendFrac = glm::min(1.0f, ac.blendFrac + dt / ac.blendDur);

            mc.mesh->animateBlend(
                ac.index, ac.nextIndex,
                ac.timeCur, ac.timeNext,
                ac.blendFrac,
                eeng::AnmationTimeFormat::RealTime,
                eeng::AnmationTimeFormat::RealTime
            );

            // Once done, commit the new clip and dequeue any next one
            if (ac.blendFrac >= 1.0f) {
                ac.index = ac.nextIndex;
                ac.timeCur = ac.timeNext;
            }
        }
        else {
            // No queued animations, just play the current one
            mc.mesh->animate(ac.index, ac.timeCur);
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

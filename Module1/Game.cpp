
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"

std::shared_ptr<eeng::RenderableMesh> InitializeMesh(std::string filePath) 
{
	auto mesh = std::make_shared<eeng::RenderableMesh>();
	mesh->load(filePath, false);
	return mesh;
}

bool Game::init()
{
    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();

    // Do some entt stuff
    entity_registry = std::make_shared<entt::registry>();

    // Grass
    grassMesh = std::make_shared<eeng::RenderableMesh>();
    grassMesh->load("assets/grass/grass_trees_merged2.fbx", false);

    // Horse
    horseMesh = std::make_shared<eeng::RenderableMesh>();
    horseMesh->load("assets/Animals/Horse.fbx", false);

    // Character
    characterMesh = std::make_shared<eeng::RenderableMesh>();
#if 0
    // Character
    characterMesh->load("assets/Ultimate Platformer Pack/Character/Character.fbx", false);
#endif
#if 0
    // Enemy
    characterMesh->load("assets/Ultimate Platformer Pack/Enemies/Bee.fbx", false);
#endif
#if 0
    // ExoRed 5.0.1 PACK FBX, 60fps, No keyframe reduction
    characterMesh->load("assets/ExoRed/exo_red.fbx");
    characterMesh->load("assets/ExoRed/idle (2).fbx", true);
    characterMesh->load("assets/ExoRed/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 1
    // Amy 5.0.1 PACK FBX
    characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    characterMesh->load("assets/Amy/idle.fbx", true) ;
    characterMesh->load("assets/Amy/walking.fbx", true);
    // Remove root motion
    characterMesh->removeTranslationKeys("mixamorig:Hips");
#endif
#if 0
    // Eve 5.0.1 PACK FBX
    // Fix for assimp 5.0.1 (https://github.com/assimp/assimp/issues/4486)
    // FBXConverter.cpp, line 648: 
    //      const float zero_epsilon = 1e-6f; => const float zero_epsilon = Math::getEpsilon<float>();
    playerMesh->load("assets/Eve/Eve By J.Gonzales.fbx");
    playerMesh->load("assets/Eve/idle.fbx", true);
    playerMesh->load("assets/Eve/walking.fbx", true);
    // Remove root motion
    playerMesh->removeTranslationKeys("mixamorig:Hips");
#endif

   

    // Fox 
    foxMesh = InitializeMesh("assets/Animals/Fox.fbx");

    // Player
    playerMesh = InitializeMesh("assets/ExoRed/exo_red.fbx");

    /*auto cameraEntity = entity_registry->create();
    entity_registry->emplace<CameraComponent>(cameraEntity, CameraComponent{});
    camera = std::make_shared<CameraComponent>(entity_registry->get<CameraComponent>(cameraEntity));*/

    auto foxEntity = entity_registry->create();
    entity_registry->emplace<MeshComponent>(foxEntity, MeshComponent{ foxMesh });
    entity_registry->emplace<LinearVelocity>(foxEntity, LinearVelocity{ {0, 0, 0} });
    entity_registry->emplace<Transform>(foxEntity, Transform{
        { 10.0f, 0.0f, 0.0f },      // position
        { 0.0f, 0.0f, 0.0f },       // rotation
        { 0.01f, 0.01f, 0.01f }     // scale
    });
    entity_registry->emplace<NpcController>(foxEntity, NpcController{ 
        { { 0, 0, 0 }, { 5, 0, 0 }, { 5, 0, 5 }, { 0, 0, 5 } }, // patrol points
        0, // current point
        2.0f // speed
    });

    auto playerEntity = entity_registry->create();
    entity_registry->emplace<MeshComponent>(playerEntity, MeshComponent{ playerMesh });
    entity_registry->emplace<LinearVelocity>(playerEntity, LinearVelocity{ {0, 0, 0} });
    entity_registry->emplace<Transform>(playerEntity, Transform{
		{ 0.0f, 0.0f, 0.0f },      // position
		{ 0.0f, 0.0f, 0.0f },       // rotation
        { 0.03f, 0.03f, 0.03f }     // scale
	});
    entity_registry->emplace<PlayerController>(playerEntity, PlayerController{
		6.0f,                           // speed
		glm::vec3(0.0f, 0.0f, -1.0f),   // forward
		glm::vec3(1.0f, 0.0f, 0.0f),    // right
		glm_aux::Ray{ glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) } // view ray
	});


 //   auto characterEntity2 = entity_registry->create();
 //   entity_registry->emplace<MeshComponent>(characterEntity2, MeshComponent{ characterMesh });
 //   entity_registry->emplace<Transform>(characterEntity2, Transform{
 //       { -3.0f, 0.0f, 0.0f },      // position
 //       { 0.0f, 0.0f, 0.0f },       // rotation
 //       { 0.03f, 0.03f, 0.03f }     // scale
 //   });
 //   auto characterEntity3 = entity_registry->create();
 //   entity_registry->emplace<MeshComponent>(characterEntity3, MeshComponent{ characterMesh });
 //   entity_registry->emplace<Transform>(characterEntity3, Transform{
	//	{ 3.0f, 0.0f, 0.0f },      // position
	//	{ 0.0f, 0.0f, 0.0f },       // rotation
 //       { 0.03f, 0.03f, 0.03f }     // scale
	//});

    grassWorldMatrix = glm_aux::TRS(
        { 0.0f, 0.0f, 0.0f },
        0.0f, { 0, 1, 0 },
        { 100.0f, 100.0f, 100.0f });

    horseWorldMatrix = glm_aux::TRS(
        { 30.0f, 0.0f, -35.0f },
        35.0f, { 0, 1, 0 },
        { 0.01f, 0.01f, 0.01f });

    return true;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{

    // Update the player controller
    auto playerView = entity_registry->view<Transform, PlayerController, LinearVelocity>();
    for (auto entity : playerView) {
        auto& transform = playerView.get<Transform>(entity);
        auto& playerController = playerView.get<PlayerController>(entity);
        auto& linearVelocity = playerView.get<LinearVelocity>(entity);
        
        //updatePlayer(transform, playerController, deltaTime, input);
        systems.PlayerControllerSystem(transform, playerController, linearVelocity, input, camera);
    }

    // Update NPC controllers
    auto npcView = entity_registry->view<Transform, NpcController, LinearVelocity>();
    for (auto entity : npcView) {
        auto& transform = npcView.get<Transform>(entity);
        auto& npcController = npcView.get<NpcController>(entity);
        auto& linearVelocity = npcView.get<LinearVelocity>(entity);

        systems.NpcControllerSystem(transform, linearVelocity, npcController);
    }

    // Update movement for entities
    auto movementView = entity_registry->view<Transform, LinearVelocity>();
    for (auto entity : movementView) {
		auto& transform = movementView.get<Transform>(entity);
		auto& linearVelocity = movementView.get<LinearVelocity>(entity);

		// Update the position of the transform based on the linear velocity
        systems.MovementSystem(transform, linearVelocity, deltaTime);
	}

    // Update Camera
    updateCamera(input);

    //auto cameraView = entity_registry->view<CameraComponent>();
    //for (auto entity : cameraView) {
	//	auto& camera = cameraView.get<CameraComponent>(entity);

	//	systems.CameraSystem(camera, input);
	//}

    pointlight.pos = glm::vec3(
        glm_aux::R(time * 0.1f, { 0.0f, 1.0f, 0.0f }) *
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f));

    characterWorldMatrix2 = glm_aux::TRS(
        { -3, 0, 0 },
        time * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

    characterWorldMatrix3 = glm_aux::TRS(
        { 3, 0, 0 },
        time * glm::radians(50.0f), { 0, 1, 0 },
        { 0.03f, 0.03f, 0.03f });

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
    if (input->GetMouseState().rightButton)
    {
        glm::ivec2 windowPos(camera.mouse_xy_prev.x, matrices.windowSize.y - camera.mouse_xy_prev.y);
        auto ray = glm_aux::world_ray_from_window_coords(windowPos, matrices.V, matrices.P, matrices.VP);
        // Intersect with e.g. AABBs ...

        eeng::Log("Picking ray origin = %s, dir = %s",
            glm_aux::to_string(ray.origin).c_str(),
            glm_aux::to_string(ray.dir).c_str());
    }
}

void Game::render(
    float time,
    int windowWidth,
    int windowHeight)
{
    gameTime = time;
    renderUI();

    matrices.windowSize = glm::ivec2(windowWidth, windowHeight);

    // Projection matrix
    const float aspectRatio = float(windowWidth) / windowHeight;
    matrices.P = glm::perspective(glm::radians(60.0f), aspectRatio, camera.nearPlane, camera.farPlane);

    // View matrix
    matrices.V = glm::lookAt(camera.pos, camera.lookAt, camera.up);

    matrices.VP = glm_aux::create_viewport_matrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);

    // Begin rendering pass
    forwardRenderer->beginPass(matrices.P, matrices.V, pointlight.pos, pointlight.color, camera.pos);

    // Rendering entities
    auto meshView = entity_registry->view<Transform, MeshComponent>();
    for (auto entity : meshView) {
		auto& transform = meshView.get<Transform>(entity);
		auto& mesh = meshView.get<MeshComponent>(entity);

        systems.RenderSystem(forwardRenderer, transform, mesh);
    }

    // Grass
    forwardRenderer->renderMesh(grassMesh, grassWorldMatrix);
    grass_aabb = grassMesh->m_model_aabb.post_transform(grassWorldMatrix);

    // Horse
    horseMesh->animate(3, time);
    forwardRenderer->renderMesh(horseMesh, horseWorldMatrix);
    horse_aabb = horseMesh->m_model_aabb.post_transform(horseWorldMatrix);

    // Character, instance 1
    /*characterMesh->animate(characterAnimIndex, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix1);
    character_aabb1 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix1);*/

    // Character, instance 2
    characterMesh->animate(1, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix2);
    character_aabb2 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix2);

    // Character, instance 3
    characterMesh->animate(2, time * characterAnimSpeed);
    forwardRenderer->renderMesh(characterMesh, characterWorldMatrix3);
    character_aabb3 = characterMesh->m_model_aabb.post_transform(characterWorldMatrix3);

    // End rendering pass
    drawcallCount = forwardRenderer->endPass();

    // Draw player view ray
    /*auto rayView = entity_registry->view<PlayerController>();
    for (auto entity : rayView) {
		auto& player = rayView.get<PlayerController>(entity);

        if (player.viewRay)
        {
            shapeRenderer->push_states(ShapeRendering::Color4u{ 0xff00ff00 });
            shapeRenderer->push_line(player.viewRay.origin, player.viewRay.point_of_contact());
        }
        else
        {
            shapeRenderer->push_states(ShapeRendering::Color4u{ 0xffffffff });
            shapeRenderer->push_line(player.viewRay.origin, player.viewRay.origin + player.viewRay.dir * 100.0f);
        }
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
	}*/

    // Draw object bases
    {
        shapeRenderer->push_basis_basic(characterWorldMatrix1, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix2, 1.0f);
        shapeRenderer->push_basis_basic(characterWorldMatrix3, 1.0f);
        shapeRenderer->push_basis_basic(grassWorldMatrix, 1.0f);
        shapeRenderer->push_basis_basic(horseWorldMatrix, 1.0f);
    }

    // Draw AABBs
    {
        shapeRenderer->push_states(ShapeRendering::Color4u{ 0xFFE61A80 });
        shapeRenderer->push_AABB(character_aabb1.min, character_aabb1.max);
        shapeRenderer->push_AABB(character_aabb2.min, character_aabb2.max);
        shapeRenderer->push_AABB(character_aabb3.min, character_aabb3.max);
        shapeRenderer->push_AABB(horse_aabb.min, horse_aabb.max);
        shapeRenderer->push_AABB(grass_aabb.min, grass_aabb.max);
        shapeRenderer->pop_states<ShapeRendering::Color4u>();
    }

#if 0
    // Demo draw other shapes
    {
        shapeRenderer->push_states(glm_aux::T(glm::vec3(0.0f, 0.0f, -5.0f)));
        ShapeRendering::DemoDraw(shapeRenderer);
        shapeRenderer->pop_states<glm::mat4>();
    }
#endif

    // Draw shape batches
    shapeRenderer->render(matrices.P * matrices.V);
    shapeRenderer->post_render();
}

void Game::renderUI()
{
    ImGui::Begin("Game Info");

    ImGui::Text("Drawcall count %i", drawcallCount);

    if (ImGui::ColorEdit3("Light color",
        glm::value_ptr(pointlight.color),
        ImGuiColorEditFlags_NoInputs))
    {
    }

    if (characterMesh)
    {
        // Combo (drop-down) for animation clip
        int curAnimIndex = characterAnimIndex;
        std::string label = (curAnimIndex == -1 ? "Bind pose" : characterMesh->getAnimationName(curAnimIndex));
        if (ImGui::BeginCombo("Character animation##animclip", label.c_str()))
        {
            // Bind pose item
            const bool isSelected = (curAnimIndex == -1);
            if (ImGui::Selectable("Bind pose", isSelected))
                curAnimIndex = -1;
            if (isSelected)
                ImGui::SetItemDefaultFocus();

            // Clip items
            for (int i = 0; i < characterMesh->getNbrAnimations(); i++)
            {
                const bool isSelected = (curAnimIndex == i);
                const auto label = characterMesh->getAnimationName(i) + "##" + std::to_string(i);
                if (ImGui::Selectable(label.c_str(), isSelected))
                    curAnimIndex = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
            characterAnimIndex = curAnimIndex;
        }

        // In-world position label
        const auto VP_P_V = matrices.VP * matrices.P * matrices.V;
        auto world_pos = glm::vec3(horseWorldMatrix[3]);
        glm::ivec2 window_coords;
        if (glm_aux::window_coords_from_world_pos(world_pos, VP_P_V, window_coords))
        {
            ImGui::SetNextWindowPos(
                ImVec2{ float(window_coords.x), float(matrices.windowSize.y - window_coords.y) },
                ImGuiCond_Always,
                ImVec2{ 0.0f, 0.0f });
            ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x80000000);
            ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoInputs |
                // ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin("window_name", nullptr, flags))
            {
                ImGui::Text("In-world GUI element");
                ImGui::Text("Window pos (%i, %i)", window_coords.x, window_coords.x);
                ImGui::Text("World pos (%1.1f, %1.1f, %1.1f)", world_pos.x, world_pos.y, world_pos.z);
                ImGui::End();
            }
            ImGui::PopStyleColor(2);
        }
    }

    ImGui::SliderFloat("Animation speed", &characterAnimSpeed, 0.1f, 5.0f);

    ImGui::End(); // end info window

    ImGui::Begin("Misc");

    ImGui::Text("Camera misc:");
    ImGui::SliderFloat("Camera distance", &camera.distance, 1.0f, 100.0f);

    ImGui::Text("Player misc:");
    auto playerView = entity_registry->view<Transform, PlayerController>();
    for (auto entity : playerView) {
		auto& playerController = playerView.get<PlayerController>(entity);
		ImGui::SliderFloat("Player speed", &playerController.speed, 0.1f, 10.0f);
        break;
	}

    ImGui::Text("NPC (fox) Misc:");
    auto npcView = entity_registry->view<Transform, NpcController>();
    for (auto entity : npcView) {
		auto& transform = npcView.get<Transform>(entity);
		auto& npcController = npcView.get<NpcController>(entity);
        ImGui::SliderFloat("NPC speed", &npcController.speed, 0.1f, 10.0f);

        float scale = transform.scale.x;
        ImGui::SliderFloat("NPC scale", &scale, 0.01f, 0.05f);
        transform.scale = { scale, scale, scale };
        break;
	}

    ImGui::Text("Miscellaneous info:");
    ImGui::Text("Game time: %1.1f", gameTime);

    ImGui::End(); // end misc window
}

void Game::destroy()
{

}

void Game::updateCamera(
    InputManagerPtr input)
{
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
}
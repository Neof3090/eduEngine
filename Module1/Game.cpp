
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "imgui.h"
#include "Log.hpp"
#include "Game.hpp"


void Game::LoadMeshes()
{
    // Player - ExoRed 5.0.1 PACK FBX, 60fps, No keyframe reduction
    playerMesh = std::make_shared<eeng::RenderableMesh>();
    playerMesh->load("assets/ExoRed/exo_red.fbx");
    playerMesh->load("assets/ExoRed/idle (2).fbx", true);
    playerMesh->load("assets/ExoRed/walking.fbx", true);
    // Remove root motion
    playerMesh->removeTranslationKeys("mixamorig:Hips");

    // Fox 
    foxMesh = std::make_shared<eeng::RenderableMesh>();
    foxMesh->load("assets/Animals/Fox.fbx", false);

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
#if 1
    // Amy 5.0.1 PACK FBX
    characterMesh->load("assets/Amy/Ch46_nonPBR.fbx");
    characterMesh->load("assets/Amy/idle.fbx", true);
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
}

bool Game::init()
{
    forwardRenderer = std::make_shared<eeng::ForwardRenderer>();
    forwardRenderer->init("shaders/phong_vert.glsl", "shaders/phong_frag.glsl");

    shapeRenderer = std::make_shared<ShapeRendering::ShapeRenderer>();
    shapeRenderer->init();

    LoadMeshes();

    // Do some entt stuff
    entity_registry = std::make_shared<entt::registry>();

    auto pointlightEntity = entity_registry->create();
    entity_registry->emplace<PointLight>(pointlightEntity, PointLight{});

    auto cameraEntity = entity_registry->create();
    entity_registry->emplace<Transform>(cameraEntity, Transform{ });
    entity_registry->emplace<CameraComponent>(cameraEntity, CameraComponent{ });

    auto playerEntity = entity_registry->create();
    entity_registry->emplace<MeshComponent>(playerEntity, MeshComponent{ 
        playerMesh,
        2, // animation index
        1.0f // animation speed
        });
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
		glm_aux::Ray{ glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) }, // view ray
        cameraEntity // camera entity
	});

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

    auto characterEntity2 = entity_registry->create();
    entity_registry->emplace<MeshComponent>(characterEntity2, MeshComponent{ 
        characterMesh,
        1, // animation index
        1.0f // animation speed
        });
    entity_registry->emplace<Transform>(characterEntity2, Transform{
        { -3.0f, 0.0f, 0.0f },      // position
        { 0.0f, 0.0f, 0.0f },       // rotation
        { 0.03f, 0.03f, 0.03f }     // scale
    });
    auto characterEntity3 = entity_registry->create();
    entity_registry->emplace<MeshComponent>(characterEntity3, MeshComponent{ 
        characterMesh,
        2,
        1.0f
        });
    entity_registry->emplace<Transform>(characterEntity3, Transform{
		{ 3.0f, 0.0f, 0.0f },      // position
		{ 0.0f, 0.0f, 0.0f },       // rotation
        { 0.03f, 0.03f, 0.03f }     // scale
	});

    auto grassEntity = entity_registry->create();
    entity_registry->emplace<MeshComponent>(grassEntity, MeshComponent{ grassMesh });
    entity_registry->emplace<Transform>(grassEntity, Transform{
		{ 0.0f, 0.0f, 0.0f },      // position
		{ 0.0f, 0.0f, 0.0f },       // rotation
		{ 100.0f, 100.0f, 100.0f }  // scale
	});

    auto horseEntity = entity_registry->create();
    entity_registry->emplace<MeshComponent>(horseEntity, MeshComponent{ horseMesh });
    entity_registry->emplace<Transform>(horseEntity, Transform{
        { 30.0f, 0.0f, -35.0f },
        { 0, 35.0f, 0 },
        { 0.01f, 0.01f, 0.01f }
    });

    return true;
}

void Game::update(
    float time,
    float deltaTime,
    InputManagerPtr input)
{
    gameTime = time;

    // Update the player controller
    systems.PlayerControllerSystem(*entity_registry, input, deltaTime);

    // Update Camera
	systems.CameraSystem(*entity_registry);

    // Update NPC controllers
    systems.NpcControllerSystem(*entity_registry);

    // Update movement for entities
    systems.MovementSystem(*entity_registry, deltaTime);

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

void Game::render(
    float time,
    int windowWidth,
    int windowHeight)
{
    renderUI();

    int drawcalls = systems.RenderSystem(*entity_registry, windowWidth, windowHeight, forwardRenderer, shapeRenderer, time);
    drawcallCount = drawcalls;
}

void Game::renderUI()
{
    ImGui::Begin("Game Info");

    ImGui::Text("Drawcall count %i", drawcallCount);

    auto lightView = entity_registry->view<PointLight>();
    if (!lightView.empty()) {
        auto lightEntity = lightView.front();
        auto& pointlight = entity_registry->get<PointLight>(lightEntity);

        ImGui::ColorEdit3("Light color", glm::value_ptr(pointlight.color), ImGuiColorEditFlags_NoInputs);
    }

    float* characterAnimSpeed = nullptr;

    if (characterMesh)
    {
        // Combo (drop-down) for animation clip
        auto view = entity_registry->view<MeshComponent>();
        for (auto entity : view) {
            auto& mesh = view.get<MeshComponent>(entity);
            if (mesh.mesh == characterMesh)
			{
                characterAnimSpeed = &mesh.animSpeed;
				int curAnimIndex = mesh.animIndex;

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

                    mesh.animIndex = curAnimIndex;
                }
				break;
			}
        }

        // In-world position label
        //const auto VP_P_V = matrices.VP * matrices.P * matrices.V;
        //auto world_pos = glm::vec3(horseWorldMatrix[3]);
        //glm::ivec2 window_coords;
        //if (glm_aux::window_coords_from_world_pos(world_pos, VP_P_V, window_coords))
        //{
        //    ImGui::SetNextWindowPos(
        //        ImVec2{ float(window_coords.x), float(matrices.windowSize.y - window_coords.y) },
        //        ImGuiCond_Always,
        //        ImVec2{ 0.0f, 0.0f });
        //    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x80000000);
        //    ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);

        //    ImGuiWindowFlags flags =
        //        ImGuiWindowFlags_NoDecoration |
        //        ImGuiWindowFlags_NoInputs |
        //        // ImGuiWindowFlags_NoBackground |
        //        ImGuiWindowFlags_AlwaysAutoResize;

        //    if (ImGui::Begin("window_name", nullptr, flags))
        //    {
        //        ImGui::Text("In-world GUI element");
        //        ImGui::Text("Window pos (%i, %i)", window_coords.x, window_coords.x);
        //        ImGui::Text("World pos (%1.1f, %1.1f, %1.1f)", world_pos.x, world_pos.y, world_pos.z);
        //        ImGui::End();
        //    }
        //    ImGui::PopStyleColor(2);
        //}
    }

    if (characterAnimSpeed) {
        ImGui::SliderFloat("Animation speed", characterAnimSpeed, 0.1f, 5.0f);
    }

    ImGui::End(); // end info window

    // Debug window
    ImGui::Begin("Debug");
    ImGui::Checkbox("Show Bones", &systems.debug.showBones);
    ImGui::Checkbox("Show AABBs", &systems.debug.showAABBs);
    ImGui::Checkbox("Show View Rays", &systems.debug.showViewRays);
    ImGui::Checkbox("Show Object Bases", &systems.debug.showObjectBases);
    ImGui::End(); // end debug window

    ImGui::Begin("Misc");

    ImGui::Text("Camera misc:");
    auto camView = entity_registry->view<CameraComponent>();
    for (auto entity : camView) {
        auto& cam = camView.get<CameraComponent>(entity);
        ImGui::SliderFloat("Camera distance", &cam.distance, 1.0f, 100.0f);
        break;
    }

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

std::shared_ptr<eeng::RenderableMesh> InitializeMesh(std::string filePath)
{
    auto mesh = std::make_shared<eeng::RenderableMesh>();
    mesh->load(filePath, false);
    return mesh;
}

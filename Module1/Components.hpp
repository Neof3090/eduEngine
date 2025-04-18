#pragma once
#include <glmcommon.hpp>
#include <RenderableMesh.hpp>
#include <memory>
#include <entt/entt.hpp>

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct LinearVelocity {
	glm::vec3 velocity;
};

struct MeshComponent {
	std::shared_ptr<eeng::RenderableMesh> mesh;
	eeng::AABB aabb; // (Axis-aligned bounding box)
	int animIndex;
	float animSpeed;
};

struct PlayerController {
	float speed = 2;
	glm::vec3 forward, right;
	glm_aux::Ray viewRay;
	entt::entity cameraEntity;

	// Previous mouse position
	glm::ivec2 mouse_xy_prev{ -1, -1 };
};

struct NpcController {
	std::vector<glm::vec3> patrolPoints;
	int currentPoint = 0;
	float speed;
};

// Basic camera structure
struct CameraComponent 
{
	entt::entity target;					// Entity to look at
	glm::vec3 lookAt = glm::vec3(0,0,0);	// Point of interest
	glm::vec3 up = glm::vec3(0,1,0);		// Local up-vector
	float distance = 15.0f;                 // Distance to point-of-interest
	float sensitivity = 0.005f;             // Mouse sensitivity
	const float nearPlane = 1.0f;           // Rendering near plane
	const float farPlane = 500.0f;          // Rendering far plane

	// Position and view angles (computed when camera is updated)
	float yaw = 0.0f;                       // Horizontal angle (radians)
	float pitch = -glm::pi<float>() / 8;    // Vertical angle (radians)
};

struct PointLight {
	glm::vec3 pos;
	glm::vec3 color{ 1.0f, 1.0f, 0.8f };
};

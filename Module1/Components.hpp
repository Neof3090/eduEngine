#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <RenderableMesh.hpp>
#include <memory>

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct LinearVelocity {
	glm::vec3 velocity;
};

struct Mesh {
	std::shared_ptr<eeng::RenderableMesh> mesh;
};

struct PlayerController {
	glm::vec3 direction;
	float speed;
};

struct NpcController {
	glm::vec3 direction;
	float speed;
};

struct PointLight {
	glm::vec3 color;
	float intensity;
};

struct Camera
{
	glm::vec3 lookAt = glm_aux::vec3_000;   // Point of interest
	glm::vec3 up = glm_aux::vec3_010;       // Local up-vector
	float distance = 15.0f;                 // Distance to point-of-interest
	float sensitivity = 0.005f;             // Mouse sensitivity
	const float nearPlane = 1.0f;           // Rendering near plane
	const float farPlane = 500.0f;          // Rendering far plane

	// Position and view angles (computed when camera is updated)
	float yaw = 0.0f;                       // Horizontal angle (radians)
	float pitch = -glm::pi<float>() / 8;    // Vertical angle (radians)
	glm::vec3 pos;                          // Camera position

	// Previous mouse position
	glm::ivec2 mouse_xy_prev{ -1, -1 };
};

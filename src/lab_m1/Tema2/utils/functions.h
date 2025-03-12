#pragma once

#include <vector>

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/glm_utils.h"

#include "lab_m1/Tema2/utils/my_camera.h"

// functie care verifica colisiunea a doua paraleipipede
bool CheckCollideBoxes(glm::vec3 box1_center, glm::vec3 box1_point_x, glm::vec3 box1_point_y, glm::vec3 box1_point_z,
	glm::vec3 box2_center, glm::vec3 box2_point_x, glm::vec3 box2_point_y, glm::vec3 box2_point_z);
Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
void MyRenderMesh(Mesh* mesh, float transparency, Shader* shader, MyCamera* camera, const glm::mat4& projectionMatrix, glm::mat4& modelMatrix);
Mesh* CreeateParallelepiped(const char* name, glm::vec3 center, glm::vec3 size, glm::vec3 color);
Mesh* CreateCone(const char* name, glm::vec3 center, float radius, float height, int segments, glm::vec3 color);
Mesh* CreateCylinder(const char* name, glm::vec3 center, float radius, float height, int segments, glm::vec3 color);
Mesh* CreateSphere(const char* name, glm::vec3 center, float radius, int slices, int stacks, glm::vec3 color);

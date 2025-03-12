#include <vector>

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"

#include "utils/glm_utils.h"

#include "lab_m1/Tema2/utils/my_camera.h"

#include "lab_m1/Tema2/utils/functions.h"


/*
	Fiecare paralelipied are 3 axe locale (x, y, z) si un centru.
	Noi stim centrul si capetele axelor, putem calula vertexii paralelipipedului.
	Calculam 15 axe (3 + 3 axe locale si 9 de produs vectorial intre ele).
	Verificam daca proiectiile pe fiecare axa se intersecteaza.
	Daca nu se interescteaza pe macar una, atunci nu se intersecteaza paralleipipedele.
*/
bool CheckCollideBoxes(glm::vec3 box1_center, glm::vec3 box1_point_x, glm::vec3 box1_point_y, glm::vec3 box1_point_z,
	glm::vec3 box2_center, glm::vec3 box2_point_x, glm::vec3 box2_point_y, glm::vec3 box2_point_z)
{
	glm::vec3 box1_offset_x = box1_point_x - box1_center;
	glm::vec3 box1_offset_y = box1_point_y - box1_center;
	glm::vec3 box1_offset_z = box1_point_z - box1_center;

	glm::vec3 box2_offset_x = box2_point_x - box2_center;
	glm::vec3 box2_offset_y = box2_point_y - box2_center;
	glm::vec3 box2_offset_z = box2_point_z - box2_center;

	glm::vec3 box1_axe_x = glm::normalize(box1_offset_x);
	glm::vec3 box1_axe_y = glm::normalize(box1_offset_y);
	glm::vec3 box1_axe_z = glm::normalize(box1_offset_z);

	glm::vec3 box2_axe_x = glm::normalize(box2_offset_x);
	glm::vec3 box2_axe_y = glm::normalize(box2_offset_y);
	glm::vec3 box2_axe_z = glm::normalize(box2_offset_z);

	std::vector<glm::vec3> axes{
		box1_axe_x, box1_axe_y, box1_axe_z,
		box2_axe_x, box2_axe_y, box2_axe_z,
		glm::cross(box1_axe_x, box2_axe_x),
		glm::cross(box1_axe_x, box2_axe_y),
		glm::cross(box1_axe_x, box2_axe_z),
		glm::cross(box1_axe_y, box2_axe_x),
		glm::cross(box1_axe_y, box2_axe_y),
		glm::cross(box1_axe_y, box2_axe_z),
		glm::cross(box1_axe_z, box2_axe_x),
		glm::cross(box1_axe_z, box2_axe_y),
		glm::cross(box1_axe_z, box2_axe_z)
	};

	std::vector<glm::vec3> box1_vertices{
		box1_center + box1_offset_x + box1_offset_y + box1_offset_z,
		box1_center + box1_offset_x + box1_offset_y - box1_offset_z,
		box1_center + box1_offset_x - box1_offset_y + box1_offset_z,
		box1_center + box1_offset_x - box1_offset_y - box1_offset_z,
		box1_center - box1_offset_x + box1_offset_y + box1_offset_z,
		box1_center - box1_offset_x + box1_offset_y - box1_offset_z,
		box1_center - box1_offset_x - box1_offset_y + box1_offset_z,
		box1_center - box1_offset_x - box1_offset_y - box1_offset_z
	};

	std::vector<glm::vec3> box2_vertices{
		box2_center + box2_offset_x + box2_offset_y + box2_offset_z,
		box2_center + box2_offset_x + box2_offset_y - box2_offset_z,
		box2_center + box2_offset_x - box2_offset_y + box2_offset_z,
		box2_center + box2_offset_x - box2_offset_y - box2_offset_z,
		box2_center - box2_offset_x + box2_offset_y + box2_offset_z,
		box2_center - box2_offset_x + box2_offset_y - box2_offset_z,
		box2_center - box2_offset_x - box2_offset_y + box2_offset_z,
		box2_center - box2_offset_x - box2_offset_y - box2_offset_z
				
	};

	for (glm::vec3& axe : axes) {
		float min1 = glm::dot(box1_vertices[0], axe);
		float max1 = min1;
		float min2 = glm::dot(box2_vertices[0], axe);
		float max2 = min2;

		for (int i = 1; i < 8; i++) {
			float dot1 = glm::dot(box1_vertices[i], axe);
			if (dot1 < min1) {
				min1 = dot1;
			}
			if (dot1 > max1) {
				max1 = dot1;
			}

			float dot2 = glm::dot(box2_vertices[i], axe);
			if (dot2 < min2) {
				min2 = dot2;
			}
			if (dot2 > max2) {
				max2 = dot2;
			}
		}

		// daca nu se intersecteaza pe macar una din axe, atunci nu se intersecteaza paralleipipedele
		if (max1 < min2 || max2 < min1) {
			return false;
		}
	}
	return true;
}

Mesh* CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// Set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// Set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// Set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	Mesh* mesh = new Mesh(name);
	mesh->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
	mesh->vertices = vertices;
	mesh->indices = indices;

	return mesh; 
}

void MyRenderMesh(Mesh* mesh, float transparency, Shader* shader, MyCamera* camera, const glm::mat4& projectionMatrix, glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Get shader location for uniform mat4 "Model"
	int model_location = shader->GetUniformLocation("Model");
	// Set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Get shader location for uniform mat4 "View"
	int view_location = shader->GetUniformLocation("View");
	// Set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Get shader location for uniform mat4 "Projection"
	int projection_location = shader->GetUniformLocation("Projection");
	// Set shader uniform "Projection" to projectionMatrix
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Get shader location for uniform float "transparency"
	int transparency_location = shader->GetUniformLocation("transparency");
	// Set shader uniform "transparency" to transparency
	glUniform1f(transparency_location, transparency);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

Mesh* CreeateParallelepiped(const char* name, glm::vec3 center, glm::vec3 size, glm::vec3 color) {
	std::vector<VertexFormat> vertices = {
		VertexFormat(center + glm::vec3(-size.x / 2, -size.y / 2, size.z / 2), color),  // 0: Bottom-Front-Left
		VertexFormat(center + glm::vec3(size.x / 2, -size.y / 2, size.z / 2), color),   // 1: Bottom-Front-Right
		VertexFormat(center + glm::vec3(size.x / 2, -size.y / 2, -size.z / 2), color),  // 2: Bottom-Back-Right
		VertexFormat(center + glm::vec3(-size.x / 2, -size.y / 2, -size.z / 2), color), // 3: Bottom-Back-Left
		VertexFormat(center + glm::vec3(-size.x / 2, size.y / 2, -size.z / 2), color),  // 4: Top-Back-Left
		VertexFormat(center + glm::vec3(size.x / 2, size.y / 2, -size.z / 2), color),   // 5: Top-Back-Right
		VertexFormat(center + glm::vec3(size.x / 2, size.y / 2, size.z / 2), color),    // 6: Top-Front-Right
		VertexFormat(center + glm::vec3(-size.x / 2, size.y / 2, size.z / 2), color)    // 7: Top-Front-Left
	};

	// Indicii pt cele 6 fete
	std::vector<unsigned int> indices = {
		// Bottom face
		0, 1, 2,    0, 2, 3,
		// Top face
		4, 5, 6,    4, 6, 7,
		// Front face
		0, 7, 6,    0, 6, 1,
		// Back face
		2, 5, 4,    2, 4, 3,
		// Left face
		0, 3, 4,    0, 4, 7,
		// Right face
		1, 6, 5,    1, 5, 2
	};

	return CreateMesh(name, vertices, indices);
}

Mesh* CreateCone(const char* name, glm::vec3 center, float radius, float height, int segments, glm::vec3 color) {
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	// varf con
	glm::vec3 apex = center + glm::vec3(0, height, 0);
	vertices.push_back(VertexFormat(apex, color)); // Vertex 0: Apex

	// centru baza
	glm::vec3 baseCenter = center;
	vertices.push_back(VertexFormat(baseCenter, color)); // Vertex 1: Base center

	// vertecsi baza
	float angleStep = 2.0f * glm::pi<float>() / segments;
	for (int i = 0; i < segments; ++i) {
		float angle = i * angleStep;
		glm::vec3 position = baseCenter + glm::vec3(radius * cos(angle), 0, radius * sin(angle));
		vertices.push_back(VertexFormat(position, color));
	}

	// indici invelis con
	for (int i = 0; i < segments; ++i) {
		int nextIndex = (i + 1) % segments + 2;
		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back(nextIndex);
	}

	// indici baza
	for (int i = 0; i < segments; ++i) {
		int nextIndex = (i + 1) % segments + 2;
		indices.push_back(1);
		indices.push_back(nextIndex);
		indices.push_back(i + 2);
	}

	return CreateMesh(name, vertices, indices);
}

Mesh* CreateCylinder(const char* name, glm::vec3 center, float radius, float height, int segments, glm::vec3 color) {
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	// centrele capetelor cilindrului
	glm::vec3 topCenter = center + glm::vec3(0, height / 2.0f, 0);
	glm::vec3 bottomCenter = center - glm::vec3(0, height / 2.0f, 0);

	vertices.push_back(VertexFormat(topCenter, color));      // Vertex 0: Top center
	vertices.push_back(VertexFormat(bottomCenter, color));   // Vertex 1: Bottom center

	float angleStep = 2.0f * glm::pi<float>() / segments;

	// vertecsi cerc sus
	for (int i = 0; i < segments; ++i) {
		float angle = i * angleStep;
		glm::vec3 topVertex = topCenter + glm::vec3(radius * cos(angle), 0, radius * sin(angle));
		vertices.push_back(VertexFormat(topVertex, color));  // Vertices [2 ... 2+segments-1]
	}

	// vertecsi cerc jos
	int bottomStart = 2 + segments;
	for (int i = 0; i < segments; ++i) {
		float angle = i * angleStep;
		glm::vec3 bottomVertex = bottomCenter + glm::vec3(radius * cos(angle), 0, radius * sin(angle));
		vertices.push_back(VertexFormat(bottomVertex, color)); // Vertices [2+segments ... 2+2*segments-1]
	}

	int topStart = 2;
	for (int i = 0; i < segments; i++) {
		int next = (i + 1) % segments;

		int topCurr = topStart + i;
		int topNext = topStart + next;
		int bottomCurr = bottomStart + i;
		int bottomNext = bottomStart + next;

		indices.push_back(topCurr);
		indices.push_back(bottomCurr);
		indices.push_back(topNext);

		indices.push_back(topNext);
		indices.push_back(bottomCurr);
		indices.push_back(bottomNext);
	}

	for (int i = 0; i < segments; i++) {
		int next = (i + 1) % segments;
		int topCurr = topStart + i;
		int topNext = topStart + next;

		indices.push_back(0);        // Top center
		indices.push_back(topNext);
		indices.push_back(topCurr);
	}

	for (int i = 0; i < segments; i++) {
		int next = (i + 1) % segments;
		int bottomCurr = bottomStart + i;
		int bottomNext = bottomStart + next;

		indices.push_back(1);        // Bottom center
		indices.push_back(bottomCurr);
		indices.push_back(bottomNext);
	}

	return CreateMesh(name, vertices, indices);
}

Mesh* CreateSphere(const char* name, glm::vec3 center, float radius, int slices, int stacks, glm::vec3 color) {
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	for (int stack = 0; stack <= stacks; ++stack) {
		float stackAngle = glm::pi<float>() / 2 - stack * glm::pi<float>() / stacks; // de la pi/2 la -pi/2
		float xy = radius * cos(stackAngle);
		float y = radius * sin(stackAngle);

		for (int slice = 0; slice <= slices; ++slice) {
			float sliceAngle = slice * 2 * glm::pi<float>() / slices;

			float x = xy * cos(sliceAngle);
			float z = xy * sin(sliceAngle);
			glm::vec3 vertexPos = center + glm::vec3(x, y, z);

			vertices.emplace_back(VertexFormat(vertexPos, color));
		}
	}

	// indici triunghiuri randare
	for (int stack = 0; stack < stacks; ++stack) {
		for (int slice = 0; slice < slices; ++slice) {
			int first = (stack * (slices + 1)) + slice;
			int second = first + slices + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
		}
	}

	return CreateMesh(name, vertices, indices);
}


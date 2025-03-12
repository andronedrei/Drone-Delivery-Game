#include "lab_m1/Tema2/elements/terrain.h"
#include "lab_m1/Tema2/utils/constants.h"
#include "lab_m1/Tema2/utils/functions.h"

#include <vector>

DroneTerrain::DroneTerrain() {
	size = MAP_SIZE;
	granularity = MAP_GRANULARITY;
    noise_height_factor = TERRAIN_NOISE_HEIGHT_FACTOR;
	max_height = TERRAIN_MAX_HEIGHT;
	color_mix_1 = TERRAIN_COLOR_MIX_1;
	color_mix_2 = TERRAIN_COLOR_MIX_2;

	GenerateGrid();
}

DroneTerrain::~DroneTerrain() {
}

void DroneTerrain::GenerateGrid() {
    int count = granularity;
    float step = size / granularity;

    vertices.reserve((count + 1) * (count + 1));
    indices.reserve(count * count * 6); // fiecare celulă are 2 triunghiuri (6 indici)

    // Generare vertecsi
    for (int i = 0; i <= count; i++) {
        for (int j = 0; j <= count; j++) {
            float x = -size * 0.5f + i * step;
            float y = 0.0f;
            float z = -size * 0.5f + j * step;

			vertices.push_back(VertexFormat(glm::vec3(x, y, z), glm::vec3(1, 1, 1)));
        }
    }

	// Generare indici
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            unsigned int v0 = i * (count + 1) + j;
            unsigned int v1 = v0 + 1;
            unsigned int v2 = (i + 1) * (count + 1) + j;
            unsigned int v3 = v2 + 1;

            // Primul triunghi: v0, v2, v1
            indices.push_back(v0);
            indices.push_back(v2);
            indices.push_back(v1);

            // Al doilea triunghi: v1, v2, v3
            indices.push_back(v1);
            indices.push_back(v2);
            indices.push_back(v3);
        }
    }

	mesh = CreateMesh("terrain", vertices, indices);
}

void DroneTerrain::RenderTerrain(Shader* shader, MyCamera* camera, const glm::mat4& projectionMatrix) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    int view_location = shader->GetUniformLocation("View");
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    int projection_location = shader->GetUniformLocation("Projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Adaugam o uniforma pentru inaltimea maxima a terenului
	int max_height_location = shader->GetUniformLocation("max_height");
	glUniform1f(max_height_location, max_height);

	// Adaugam doua uniforme pentru culorile de mixare
	int color_mix_1_location = shader->GetUniformLocation("color_mix_1");
	glUniform3fv(color_mix_1_location, 1, glm::value_ptr(color_mix_1));

	int color_mix_2_location = shader->GetUniformLocation("color_mix_2");
	glUniform3fv(color_mix_2_location, 1, glm::value_ptr(color_mix_2));

	// Desenam obiectul
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

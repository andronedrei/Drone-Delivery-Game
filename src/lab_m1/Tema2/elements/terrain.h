#pragma once

#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"

#include "lab_m1/Tema2/utils/my_camera.h"

#include <vector>

class DroneTerrain {
  protected:
	float size;
	int granularity;
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	Mesh* mesh;

	float noise_height_factor;
	float max_height;
	glm::vec3 color_mix_1;
	glm::vec3 color_mix_2;

	void GenerateGrid();

  public:
	  DroneTerrain();
	  ~DroneTerrain();

	  void RenderTerrain(Shader* shader, MyCamera* camera, const glm::mat4& projectionMatrix);
};
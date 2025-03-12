#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/glm_utils.h"

#include "vector"

#include "lab_m1/Tema2/utils/my_camera.h"

const int ALL = -1; // pt a aplica o transformare pe toate obiectele

// structura cu date despre transformari suplimentare ale componentelor unui mesh
struct MeshData {
	glm::vec3 angle; // unghiuri de rotatie ale unui mesh
	glm::vec3 scale_factor; // scalari pe cele 3 axe
	glm::vec3 reference_offset; // offsetul fata de punctul de referinta
	glm::mat4 model_mat; // matricea de modelare a mesh-ului
};

/* Pentru a detecta coliziunile "abstractizam" obiectele la paralelipipede sau sfere
Fiecarui obiect ii corespunde un paralelipiped de coliziune si o sfera de coliziune */
struct CollideData {
	int num_collide_boxes;

	glm::vec3 *collide_box_center_offset; // offset-ul fata de punctul de referinta
	glm::vec3 *half_size; // jumatatile lungimilor pe cele 3 axe

	// pentru sfera de coliziune (o aproximare "grosiera" a obiectului)
	glm::vec3 collide_sphere_center_offset;
	float radius;
};

class ScreenElement {
  protected:
	int nr_meshes;
	std::vector<Mesh*> meshes;
	MeshData* mesh_data;
	CollideData collide_data;

	bool is_visible;
	glm::vec3 world_pos; // positia punctului de referinta din obiect in lume
	glm::vec3 angle; // unghiuri de rotatie ale obiectului
	glm::vec3 scale_factor; // scalari pe cele 3 axe ale intregului obiect

	glm::vec3 forward_dir;
	glm::vec3 up_dir;
	glm::vec3 right_dir;

	glm::mat4 base_model_mat; // matricea de modelare de baza (se pot aplica peste transformari particulare)
	glm::mat4 all_rotations_mat; // matricea de rotatii a obiectului

	virtual void InitCollideData() = 0;

	void UpdateLocalDirs();

	// foloseste ALL pentru a aplica o transformare pe toate obiectele
	void Translate(int component, glm::vec3 direction);
	void RotateOX(int component, float angle);
	void RotateOY(int component, float angle);
	void RotateOZ(int component, float angle);

	void Init(int nr_meshes);

  public:
	ScreenElement();
	~ScreenElement();
	void PrepareData();
	void Render(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix);
	void Show();
	void Hide();

	glm::vec3 GetWorldPos();
	glm::mat4 GetModelMatrix();
	glm::vec3 GetForwardDir();
	glm::vec3 GetUpDir();
	void SetObjectAngleX(float angle);
	void SetObjectAngleY(float angle);
	void SetObjectAngleZ(float angle);
	void SetScale(int component, glm::vec3 scale);
	void SetModelMatrix(glm::mat4 model_mat);
	void SetAllRotationsMat(glm::mat4 all_rotations_mat);

	bool CheckCollision(ScreenElement* other);

	void ScreenElement::RotateRightDir(float angle);
	void ScreenElement::RotateUpDir(float angle);
	void ScreenElement::RotateForwardDir(float angle);
};
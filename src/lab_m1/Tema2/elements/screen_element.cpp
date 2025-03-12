#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/glm_utils.h"

#include "lab_m1/Tema2/elements/screen_element.h"
#include "lab_m1/Tema2/utils/my_camera.h"
#include "lab_m1/Tema2/utils/functions.h"
#include "lab_m1/Tema2/utils/constants.h"
#include "lab_m1/Tema2/utils/transform3D.h"

#include <vector>
#include <iostream>

ScreenElement::ScreenElement() {
	nr_meshes = 0;
	mesh_data = nullptr;
}

ScreenElement::~ScreenElement() {
	delete[] mesh_data;

	if (collide_data.collide_box_center_offset != nullptr) {
		delete[] collide_data.collide_box_center_offset;
	}
	if (collide_data.half_size != nullptr) {
		delete[] collide_data.half_size;
	}
}

void ScreenElement::Init(int nr_meshes) {
	this->nr_meshes = nr_meshes;
	mesh_data = new MeshData[nr_meshes];

	collide_data.num_collide_boxes = 0;
	collide_data.collide_box_center_offset = nullptr;
	collide_data.half_size = nullptr;
	collide_data.collide_sphere_center_offset = glm::vec3(0);
	collide_data.radius = 0;

	// VEZI LA MESH-uri
	meshes.resize(nr_meshes);

	is_visible = true;
	world_pos = ORIGIN;
	angle = glm::vec3(0);
	scale_factor = glm::vec3(1);
	base_model_mat = glm::mat4(1);
	all_rotations_mat = glm::mat4(1);

	forward_dir = glm::vec3(0, 0, -1);
	up_dir = glm::vec3(0, 1, 0);
	right_dir = glm::vec3(1, 0, 0);

	// initializam si componentele obiectului
	for (int i = 0; i < nr_meshes; i++) {
		mesh_data[i].angle = glm::vec3(0);
		mesh_data[i].scale_factor = glm::vec3(1);
		mesh_data[i].reference_offset = glm::vec3(0);
		mesh_data[i].model_mat = glm::mat4(1);
	}
}

void ScreenElement::PrepareData() {
	if (!is_visible) {
		return;
	}

	base_model_mat = glm::mat4(1);

	// calulam transformarile aplicate pe tot obiectul
	base_model_mat *= transform3D::Translate(world_pos);
	//all_rotations_mat *= transform3D::RotateOX(angle.x);
	//all_rotations_mat *= transform3D::RotateOY(angle.y);
	//all_rotations_mat *= transform3D::RotateOZ(angle.z);
	base_model_mat *= all_rotations_mat;
	base_model_mat *= transform3D::RotateOZ(angle.z);
	base_model_mat *= transform3D::Scale(scale_factor);

	// calculam trasnformarile aplicate pe componentele obiectului
	for (int i = 0; i < nr_meshes; i++) {
		mesh_data[i].model_mat = base_model_mat;

		float deltaOX, deltaOY, deltaOZ;
		deltaOX = mesh_data[i].angle.x;
		deltaOY = mesh_data[i].angle.y;
		deltaOZ = mesh_data[i].angle.z;

		mesh_data[i].model_mat *= transform3D::Translate(mesh_data[i].reference_offset);
		mesh_data[i].model_mat *= transform3D::RotateOX(deltaOX);
		mesh_data[i].model_mat *= transform3D::RotateOY(deltaOY);
		mesh_data[i].model_mat *= transform3D::RotateOZ(deltaOZ);
		mesh_data[i].model_mat *= transform3D::Scale(mesh_data[i].scale_factor);
	}
}

void ScreenElement::Render(Shader* shader, MyCamera* camera, glm::mat4& projectionMatrix) {
	if (!is_visible) {
		return;
	}

	// randam meshurile obiectului
	for (int i = 0; i < nr_meshes; i++) {
		MyRenderMesh(meshes[i], NO_TRANSPARENCY, shader, camera, projectionMatrix, mesh_data[i].model_mat);
	}
}

void ScreenElement::Show() {
	is_visible = true;
}

void ScreenElement::Hide() {
	is_visible = false;
}

glm::vec3 ScreenElement::GetWorldPos() {
	return world_pos;
}

glm::mat4 ScreenElement::GetModelMatrix() {
	return base_model_mat;
}

glm::vec3 ScreenElement::GetForwardDir() {
	return forward_dir;
}

glm::vec3 ScreenElement::GetUpDir() {
	return up_dir;
}

void ScreenElement::SetObjectAngleX(float angle) {
	this->angle.x = angle;
}

void ScreenElement::SetObjectAngleY(float angle) {
	this->angle.y = angle;
}

void ScreenElement::SetObjectAngleZ(float angle) {
	this->angle.z = angle;
}

void ScreenElement::SetAllRotationsMat(glm::mat4 all_rotations_mat) {
	this->all_rotations_mat = all_rotations_mat;
	UpdateLocalDirs();
}

bool ScreenElement::CheckCollision(ScreenElement* other) {
	// verificăm dacă unul dintre obiecte nu este vizibil
	if (!is_visible || !other->is_visible) {
		return false;
	}

	// verificăm coliziunea sferică (aceasta ne spune doar daca este posibil sa existe coliziune)
	glm::vec3 center1 = world_pos + collide_data.collide_sphere_center_offset;
	glm::vec3 center2 = other->world_pos + other->collide_data.collide_sphere_center_offset;
	float dist = glm::distance(center1, center2);
	float radiuses_sum = collide_data.radius + other->collide_data.radius;
	if (dist > radiuses_sum) {
		// evitam restul calculelor
		return false;
	}

	glm::mat4 model_mat1 = this->base_model_mat; // matrice model obiect curent
	glm::mat4 model_mat2 = other->base_model_mat; // matrice model obiect cu care verificam coliziunea

	/* Calculăm punctele necesare pentru cutia de coliziune a primului obiect, adica
	centrul local al cutiei și axele cu jumătate din dimensiune */
	for (int i = 0; i < other->collide_data.num_collide_boxes; i++) {
		/* In mod normal ar trebui un doublu for (iterand prin collide box - urile ambelor obiecte),
		dar noi vom apela aceasta functie doar din cadrul obiectelor de tip drona cu un collide box, asa ca punem index "0" */
		glm::vec3 b1_base = collide_data.collide_box_center_offset[0];
		glm::vec3 b1_x = b1_base + glm::vec3(collide_data.half_size[0].x, 0, 0);
		glm::vec3 b1_y = b1_base + glm::vec3(0, collide_data.half_size[0].y, 0);
		glm::vec3 b1_z = b1_base + glm::vec3(0, 0, collide_data.half_size[0].z);

		// Aplicăm transformarea globală
		b1_base = glm::vec3(model_mat1 * glm::vec4(b1_base, 1.0f));
		b1_x = glm::vec3(model_mat1 * glm::vec4(b1_x, 1.0f));
		b1_y = glm::vec3(model_mat1 * glm::vec4(b1_y, 1.0f));
		b1_z = glm::vec3(model_mat1 * glm::vec4(b1_z, 1.0f));

		// Pentru celălalt obiect
		glm::vec3 b2_base = other->collide_data.collide_box_center_offset[i];
		glm::vec3 b2_x = b2_base + glm::vec3(other->collide_data.half_size[i].x, 0, 0);
		glm::vec3 b2_y = b2_base + glm::vec3(0, other->collide_data.half_size[i].y, 0);
		glm::vec3 b2_z = b2_base + glm::vec3(0, 0, other->collide_data.half_size[i].z);

		b2_base = glm::vec3(model_mat2 * glm::vec4(b2_base, 1.0f));
		b2_x = glm::vec3(model_mat2 * glm::vec4(b2_x, 1.0f));
		b2_y = glm::vec3(model_mat2 * glm::vec4(b2_y, 1.0f));
		b2_z = glm::vec3(model_mat2 * glm::vec4(b2_z, 1.0f));

		// Verificăm coliziunea OBB-OBB folosind CheckCollideBoxes
		if (CheckCollideBoxes(b1_base, b1_x, b1_y, b1_z, b2_base, b2_x, b2_y, b2_z)) {
			return true;
		}
	}

	return false;
}

void ScreenElement::Translate(int component, glm::vec3 direction) {
	component == ALL ? world_pos += direction : mesh_data[component].reference_offset += direction;
}

void ScreenElement::UpdateLocalDirs() {
	forward_dir = glm::vec3(all_rotations_mat * glm::vec4(glm::vec3(0, 0, -1), 1.0f));
	up_dir = glm::vec3(all_rotations_mat * glm::vec4(glm::vec3(0, 1, 0), 1.0f));
	right_dir = glm::vec3(all_rotations_mat * glm::vec4(glm::vec3(1, 0, 0), 1.0f));

	forward_dir = glm::normalize(forward_dir);
	up_dir = glm::normalize(up_dir);
	right_dir = glm::normalize(right_dir);
}

void ScreenElement::RotateOX(int component, float angle) {
	if (component == ALL) {
		this->angle.x += angle;
		all_rotations_mat *= transform3D::RotateOX(angle);
		UpdateLocalDirs();
	} else {
		mesh_data[component].angle.x += angle;
	}
}

void ScreenElement::RotateOY(int component, float angle) {
	if (component == ALL) {
		this->angle.y += angle;
		all_rotations_mat *= transform3D::RotateOY(angle);
		UpdateLocalDirs();
	} else {
		mesh_data[component].angle.y += angle;
	}
}

void ScreenElement::RotateOZ(int component, float angle) {
	if (component == ALL) {
		this->angle.z += angle;
		all_rotations_mat *= transform3D::RotateOZ(angle);
		UpdateLocalDirs();
	}
	else {
		mesh_data[component].angle.z += angle;
	}
}

void ScreenElement::SetScale(int component, glm::vec3 scale) {
	component == ALL ? scale_factor = scale : mesh_data[component].scale_factor = scale;
}

void ScreenElement::SetModelMatrix(glm::mat4 model_mat) {
	base_model_mat = model_mat;
}

// rotatie in jurul axei proprii RightDir (poate roti doar intregul obiect)
void ScreenElement::RotateRightDir(float angle) {
	glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, right_dir);
	all_rotations_mat = rotation * all_rotations_mat;
	UpdateLocalDirs();
}

void ScreenElement::RotateUpDir(float angle) {
	glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, up_dir);
	all_rotations_mat = rotation * all_rotations_mat;
	UpdateLocalDirs();
}

void ScreenElement::RotateForwardDir(float angle) {
	glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, forward_dir);
	all_rotations_mat = rotation * all_rotations_mat;
	UpdateLocalDirs();
}